-- fsm.vhd: Finite State Machine
-- Author(s): Nikolas Masica xmasic00
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1, TEST2, TEST3_CODE1, TEST4_CODE1, TEST5_CODE1, TEST6_CODE1, TEST7_CODE1, TEST8_CODE1, TEST9_CODE1, TEST10_CODE1, TEST3_CODE2, TEST4_CODE2, TEST5_CODE2, TEST6_CODE2, TEST7_CODE2, TEST8_CODE2, TEST9_CODE2, TEST10_CODE2, TEST11, PRINT_RIGHTMSG, PRINT_WRONGMSG, WRONG, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      next_state <= TEST1;
      if (KEY(15) = '1') then
        next_state <= PRINT_WRONGMSG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= WRONG;
      if (KEY(1) = '1') then
        next_state <= TEST2;
      end if;
      end if;

   when TEST2 =>
      next_state <= TEST2;
      if (KEY(15) = '1') then
        next_state <= PRINT_WRONGMSG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= WRONG;
      if (KEY(7) = '1') then
        next_state <= TEST3_CODE1;
      end if;
      if (KEY(9) = '1') then
        next_state <= TEST3_CODE2;
      end if;
      end if;

    when TEST3_CODE1 =>
        next_state <= TEST3_CODE1;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(2) = '1') then
            next_state <= TEST4_CODE1;
        end if;
        end if;

    when TEST4_CODE1 =>
        next_state <= TEST4_CODE1;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG;
        if (KEY(5) = '1') then
            next_state <= TEST5_CODE1;
        end if;
        end if;

    when TEST5_CODE1 =>
        next_state <= TEST5_CODE1;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(1) = '1') then
            next_state <= TEST6_CODE1;
        end if;
        end if;

    when TEST6_CODE1 =>
        next_state <= TEST6_CODE1;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(7) = '1') then
            next_state <= TEST7_CODE1;
        end if;
        end if;

    when TEST7_CODE1 =>
        next_state <= TEST7_CODE1;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(4) = '1') then
            next_state <= TEST8_CODE1;
        end if;
        end if;

    when TEST8_CODE1 =>
        next_state <= TEST8_CODE1;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(5) = '1') then
            next_state <= TEST9_CODE1;
        end if;
        end if;

    when TEST9_CODE1 =>
        next_state <= TEST9_CODE1;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(2) = '1') then
            next_state <= TEST10_CODE1;
        end if;
        end if;

    when TEST10_CODE1 =>
        next_state <= TEST10_CODE1;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(2) = '1') then
            next_state <= TEST11;
        end if;
        end if;

    when TEST3_CODE2 =>
        next_state <= TEST3_CODE2;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(7) = '1') then
            next_state <= TEST4_CODE2;
        end if;
        end if;

    when TEST4_CODE2 =>
        next_state <= TEST4_CODE2;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(1 downto 0) /= "000000000000000") then
         next_state <= WRONG;
        if (KEY(1) = '1') then
            next_state <= TEST5_CODE2;
        end if;
        end if;

    when TEST5_CODE2 =>
        next_state <= TEST5_CODE2;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(6) = '1') then
            next_state <= TEST6_CODE2;
        end if;
        end if;

    when TEST6_CODE2 =>
        next_state <= TEST6_CODE2;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(2) = '1') then
            next_state <= TEST7_CODE2;
        end if;
        end if;

    when TEST7_CODE2 =>
        next_state <= TEST7_CODE2;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(8) = '1') then
            next_state <= TEST8_CODE2;
        end if;
        end if;

    when TEST8_CODE2 =>
        next_state <= TEST8_CODE2;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(0) = '1') then
            next_state <= TEST9_CODE2;
        end if;
        end if;

    when TEST9_CODE2 =>
        next_state <= TEST9_CODE2;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(2) = '1') then
            next_state <= TEST10_CODE2;
        end if;
        end if;

    when TEST10_CODE2 =>
        next_state <= TEST10_CODE2;
        if (KEY(15) = '1') then
            next_state <= PRINT_WRONGMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        if (KEY(5) = '1') then
            next_state <= TEST11;
        end if;
        end if;

    when TEST11 =>
        next_state <= TEST11;
        if (KEY(15) = '1') then
            next_state <= PRINT_RIGHTMSG;
        elsif (KEY(14 downto 0) /= "000000000000000") then
            next_state <= WRONG;
        end if;

    when WRONG =>
        next_state <= WRONG;
    	if (KEY(15) = '1') then
    		next_state <= PRINT_WRONGMSG;
    	end if;

	when PRINT_RIGHTMSG =>
		next_state <= PRINT_RIGHTMSG;
    	if (CNT_OF = '1') then
			next_state <= FINISH;
    	end if;

    when PRINT_WRONGMSG =>
		next_state <= PRINT_WRONGMSG;
		if (CNT_OF = '1') then
			next_state <= FINISH;
		end if;

	when FINISH =>
		next_state <= FINISH;
		if (KEY(15) = '1') then
			next_state <= TEST1;
    	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
		FSM_CNT_CE		<= '0';
		FSM_MX_MEM		<= '0';
		FSM_MX_LCD		<= '0';
		FSM_LCD_WR		<= '0';
		FSM_LCD_CLR		<= '0';

case (present_state) is
	when PRINT_RIGHTMSG =>
		FSM_CNT_CE		<= '1';
		FSM_MX_MEM		<= '1';
		FSM_MX_LCD		<= '1';
		FSM_LCD_WR		<= '1';
	when PRINT_WRONGMSG =>
		FSM_CNT_CE		<= '1';
		FSM_MX_MEM		<= '0';
		FSM_MX_LCD		<= '1';
		FSM_LCD_WR		<= '1';
	when FINISH =>
		if (KEY(15) = '1') then
			FSM_LCD_CLR	<= '1';
		end if;
	when others =>
		if (KEY(14 downto 0) /= "000000000000000") then
			FSM_LCD_WR	<= '1';
		end if;
		if (KEY(15) = '1') then
			FSM_LCD_CLR	<= '1';
		end if;
	end case;
end process output_logic;

end architecture behavioral;