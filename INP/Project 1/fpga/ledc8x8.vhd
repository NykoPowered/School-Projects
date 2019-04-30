library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
    port (
        RESET : in std_logic;
        SMCLK : in std_logic;
        ROW : out std_logic_vector(0 to 7);
        LED : out std_logic_vector(0 to 7)
    );
end ledc8x8;

architecture main of ledc8x8 is
	
    signal cnt: std_logic_vector(11 downto 0) := (others => '0');
	signal cnt2: std_logic_vector(20 downto 0) := (others => '0');
    signal ce: std_logic;
	signal state: std_logic_vector (1 downto 0) := "00";
	signal ledset: std_logic_vector(7 downto 0) := (others => '0');
	signal rowset: std_logic_vector(7 downto 0) := (others => '0');
begin
	
	generator_ce: process(SMCLK,RESET)
		begin
			if RESET = '1' then
				cnt <= (others => '0');
			elsif rising_edge(SMCLK) then
				cnt <= cnt+1;
			end if;
		end process generator_ce;
		ce <= '1' when cnt = "111000010000" else '0';

	state_change: process(SMCLK,RESET,ce)
	begin
		if RESET = '1' then
				cnt2 <= (others => '0');
			elsif rising_edge(SMCLK)then
			    cnt2 <= cnt2+1;
				if cnt2 = "111000010000000000000" then
				state <= state+1;
				cnt2 <= (others => '0');
				end if;
			end if;
		end process state_change;
	
	row_rotation: process (SMCLK,RESET,ce)
	begin
		if RESET = '1' then
			rowset <= "10000000";
		elsif rising_edge(SMCLK) and ce = '1' then
			rowset <= rowset(0) & rowset(7 downto 1);
		end if;
	end process row_rotation;
	
	decoder: process(rowset,state)
	begin
		if state = "00" then
		case rowset is
			when "10000000" => ledset <= "00111100";
			when "01000000" => ledset <= "00111100";
			when "00100000" => ledset <= "00011100";
			when "00010000" => ledset <= "00001100";
			when "00001000" => ledset <= "00100100";
			when "00000100" => ledset <= "00110000";
			when "00000010" => ledset <= "00111000";
			when "00000001" => ledset <= "00111100";
			when others => ledset <= "11111111";
		end case;
		end if;
		
		if state = "01" then
		case rowset is
			when "10000000" => ledset <= "11111111";
            when "01000000" => ledset <= "11111111";
            when "00100000" => ledset <= "11111111";
            when "00010000" => ledset <= "11111111";
            when "00001000" => ledset <= "11111111";
            when "00000100" => ledset <= "11111111";
            when "00000010" => ledset <= "11111111";
            when "00000001" => ledset <= "11111111";
            when others => ledset <= "11111111";
        end case;
		end if;
		
		if state = "10" then
		case rowset is
			when "10000000" => ledset <= "00111100";
			when "01000000" => ledset <= "00011000";
			when "00100000" => ledset <= "00000000";
			when "00010000" => ledset <= "00100100";
			when "00001000" => ledset <= "00111100";
			when "00000100" => ledset <= "00111100";
			when "00000010" => ledset <= "00111100";
			when "00000001" => ledset <= "00111100";
			when others => ledset <= "11111111";
			end case;
		end if;
		
		if state = "11" then
        case rowset is
        	when "10000000" => ledset <= "11111111";
            when "01000000" => ledset <= "11111111";
            when "00100000" => ledset <= "11111111";
            when "00010000" => ledset <= "11111111";
            when "00001000" => ledset <= "11111111";
            when "00000100" => ledset <= "11111111";
            when "00000010" => ledset <= "11111111";
            when "00000001" => ledset <= "11111111";
            when others => ledset <= "11111111";
        end case;
        end if;
	end process decoder;
	
	LED <= ledset;
	ROW <= rowset;

end main;
