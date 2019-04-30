-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2018 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xmasic00
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='1') / zapis do pameti (DATA_RDWR='0')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
    
    signal mltpx: std_logic_vector(1 downto 0):= "00";
    signal temporary: std_logic_vector(7 downto 0) := (others => '0');
    signal cntValue: std_logic_vector(7 downto 0) := (others => '0');
    signal cntValueInc: std_logic := '0';
    signal cntValueDec: std_logic := '0';
    signal ptrValue: std_logic_vector(9 downto 0) := (others => '0');
    signal ptrValueInc: std_logic := '0';
    signal ptrValueDec: std_logic := '0';
    signal pcValue: std_logic_vector(11 downto 0) := (others => '0');
    signal pcValueInc: std_logic := '0';
    signal pcValueDec: std_logic := '0';
    
    type FSMstate is (
        FSMstate_reset, 
        FSMstate_load,
        FSMstate_add, 
        FSMstate_sub,
        FSMstate_celladd, FSMstate_celladd2, 
        FSMstate_cellsub, FSMstate_cellsub2,
        FSMstate_putchar1, FSMstate_putchar2, 
        FSMstate_getchar,
        FSMstate_while1, FSMstate_while2, FSMstate_while3, FSMstate_while4,
        FSMstate_whileend1, FSMstate_whileend2, FSMstate_whileend3, FSMstate_whileend4, FSMstate_whileend5, FSMstate_whileend6,
        FSMstate_comment1, FSMstate_comment2, FSMstate_comment3,
        FSMstate_numbertohexa, 
        FSMstate_lettertohexa,
        FSMstate_return, 
        FSMstate_others
 
    );
    signal current_state: FSMstate;
    signal new_state: FSMstate;
 
begin

-- Multiplexor MX k volbe hodnoty zapisanej do pamate dat. 
process(IN_DATA, DATA_RDATA, mltpx, temporary)
begin
    case(mltpx) is
        when "00" => DATA_WDATA <= IN_DATA;
        when "01" => DATA_WDATA <= DATA_RDATA + 1;
        when "10" => DATA_WDATA <= DATA_RDATA - 1;
        when "11" => DATA_WDATA <= temporary;
        when others =>
    end case;
end process;

-- Ukazatel do pamate dat.
ptrCounter: process(CLK, RESET, ptrValue, ptrValueInc, ptrValueDec)
begin 
    if RESET = '1' then
        ptrValue <= "0000000000";
    elsif CLK'event and CLK = '1' then
        if ptrValueInc = '1' then
            ptrValue <= ptrValue + 1;
        elsif ptrValueDec = '1' then
            ptrValue <= ptrValue - 1;
        end if;
    end if;
 
    DATA_ADDR <= ptrValue;
 
end process;

-- Urcenie zaciatku a konca prikazu while, citanie poctu zatvoriek. 
cntCounter: process(CLK, RESET, cntValueInc, cntValueDec)
begin
    if RESET = '1' then
        cntValue <= "00000000";
    elsif CLK'event and CLK = '1' then
        if cntValueInc = '1' then
            cntValue <= cntValue + 1;
        elsif cntValueDec = '1' then
            cntValue <= cntValue - 1;
        end if;
    end if;
end process;

-- Programovy citac.
pcCounter: process(CLK, RESET, pcValue, pcValueInc, pcValueDec)
begin
    if RESET = '1' then
        pcValue <= "000000000000";
    elsif CLK'event and CLK = '1' then
        if pcValueInc = '1' then
            pcValue <= pcValue + 1;
        elsif pcValueDec = '1' then
            pcValue <= pcValue - 1;
        end if;
    end if;
    CODE_ADDR <= pcValue;
end process;
 
process(CLK, RESET)
begin
    if RESET = '1' then
        current_state <= FSMstate_reset;
    elsif CLK'event and CLK = '1' then
        if EN = '1' then
            current_state <= new_state;
        end if;
    end if;
end process;
 
decoder: process(CODE_DATA, IN_VLD, OUT_BUSY, DATA_RDATA, cntValue, current_state)
begin 
    pcValueInc <= '0';
    pcValueDec <= '0';
    ptrValueInc <= '0';
    ptrValueDec <= '0';
    cntValueInc <= '0';
    cntValueDec <= '0';
    mltpx <= "00";
    CODE_EN <= '1';
    DATA_RDWR <= '0';
    DATA_EN <= '0';
    OUT_WE <= '0';
    IN_REQ <= '0';
       
    case current_state is
        when FSMstate_reset =>
            CODE_EN <= '1';
            new_state <= FSMstate_load;
         when FSMstate_load =>
            case(CODE_DATA) is
                when X"3E" => 
                    new_state <= FSMstate_add;
                when X"3C" => 
                    new_state <= FSMstate_sub;
                when X"2B" => 
                    new_state <= FSMstate_celladd;
                when X"2D" => 
                    new_state <= FSMstate_cellsub;
                when X"5B" => 
                    new_state <= FSMstate_while1;
                when X"5D" => 
                    new_state <= FSMstate_whileend1;
                when X"2E" => 
                    new_state <= FSMstate_putchar1;                    
                when X"2C" => 
                    new_state <= FSMstate_getchar;
                when X"23" => 
                    new_state <= FSMstate_comment1;
                when X"30" => 
                    new_state <= FSMstate_numbertohexa;
                when X"31" => 
                    new_state <= FSMstate_numbertohexa;
                when X"32" =>
                    new_state <= FSMstate_numbertohexa;
                when X"33" => 
                    new_state <= FSMstate_numbertohexa;
                when X"34" => 
                    new_state <= FSMstate_numbertohexa;
                when X"35" => 
                    new_state <= FSMstate_numbertohexa;
                when X"36" => 
                    new_state <= FSMstate_numbertohexa;
                when X"37" => 
                    new_state <= FSMstate_numbertohexa;
                when X"38" => 
                    new_state <= FSMstate_numbertohexa;
                when X"39" => 
                    new_state <= FSMstate_numbertohexa;
                when X"41" => 
                    new_state <= FSMstate_lettertohexa;
                when X"42" => 
                    new_state <= FSMstate_lettertohexa;
                when X"43" => 
                    new_state <= FSMstate_lettertohexa;
                when X"44" => 
                    new_state <= FSMstate_lettertohexa;
                when X"45" =>
                    new_state <= FSMstate_lettertohexa;
                when X"46" => 
                    new_state <= FSMstate_lettertohexa;
                when X"00" => 
                    new_state <= FSMstate_return;
                when others => 
                    new_state <= FSMstate_others;
            end case;
        when FSMstate_add =>
            pcValueInc <= '1';
            ptrValueInc <= '1';
            new_state <= FSMstate_reset;
        when FSMstate_sub =>
            pcValueInc <= '1';
            ptrValueDec <= '1';
            new_state <= FSMstate_reset;
        when FSMstate_celladd =>
            DATA_RDWR <= '1';
            DATA_EN <= '1';
            new_state <= FSMstate_celladd2;
        when FSMstate_celladd2 =>
            pcValueInc <= '1';
            DATA_RDWR <= '0';
            DATA_EN <= '1';
            mltpx <= "01";
            new_state <= FSMstate_reset;
        when FSMstate_cellsub =>
            DATA_RDWR <= '1';
            DATA_EN <= '1';
            new_state <= FSMstate_cellsub2;
        when FSMstate_cellsub2 =>
            DATA_RDWR <= '0';
            DATA_EN <= '1';
            pcValueInc <= '1';
            mltpx <= "10";
            new_state <= FSMstate_reset;
        when FSMstate_putchar1 =>
            if OUT_BUSY = '1' then
                new_state <= FSMstate_putchar1;
            else
                DATA_RDWR <= '1';
                DATA_EN <= '1';
                new_state <= FSMstate_putchar2;
            end if;
        when FSMstate_putchar2 =>
            OUT_DATA <= DATA_RDATA;
            OUT_WE <= '1';
            pcValueInc <= '1';
            new_state <= FSMstate_reset;
        when FSMstate_getchar =>
            IN_REQ <= '1';
            if IN_VLD = '0' then
                new_state <= FSMstate_getchar;
            else
                DATA_EN <= '1';
                mltpx <= "00";
                pcValueInc <= '1';
                DATA_RDWR <= '0';
                new_state <= FSMstate_reset;
            end if;
        when FSMstate_while1 =>
            DATA_EN <= '1';
            pcValueInc <= '1';
            DATA_RDWR <= '1';
            new_state <= FSMstate_while2;
        when FSMstate_while2 =>
            if DATA_RDATA = "00000000" then
                cntValueInc <= '1';
                new_state <= FSMstate_while3;
            else
                new_state <= FSMstate_reset;
            end if;
        when FSMstate_while3 =>
            if cntValue = "00000000" then
                new_state <= FSMstate_reset;
            else
                CODE_EN <= '1';
                new_state <= FSMstate_while4;
            end if;
        when FSMstate_while4 =>
            if CODE_DATA = X"5B" then
                cntValueInc <= '1';
            elsif CODE_DATA = X"5D" then
                 cntValueDec <= '1';
            end if;
            pcValueInc <= '1';
            new_state <= FSMstate_while3;
        when FSMstate_whileend1 =>
            DATA_RDWR <= '1';
            DATA_EN <= '1';
            new_state <= FSMstate_whileend2;
        when FSMstate_whileend2 =>
            if DATA_RDATA = "00000000" then
                pcValueInc <= '1';
                new_state <= FSMstate_reset;
            else
                new_state <= FSMstate_whileend3;
            end if;
        when FSMstate_whileend3 =>
            pcValueDec <= '1';            
            cntValueInc <= '1';
            new_state <= FSMstate_whileend4;
        when FSMstate_whileend4 =>
            if cntValue = "00000000" then
                new_state <= FSMstate_reset;
            else
                CODE_EN <= '1';
                new_state <= FSMstate_whileend5;
            end if;
        when FSMstate_whileend5 =>
            if CODE_DATA = X"5D" then
                cntValueInc <= '1';
            elsif CODE_DATA = X"5B" then
                cntValueDec <= '1';
            end if;
            new_state <= FSMstate_whileend6;
        when FSMstate_whileend6 =>
            if cntValue = "00000000" then
                pcValueInc <= '1';
            else
                pcValueDec <= '1';
            end if;
            new_state <= FSMstate_whileend4;
        when FSMstate_comment1 =>
            pcValueInc <= '1';
            new_state <= FSMstate_comment2;
        when FSMstate_comment2 =>
            CODE_EN <= '1';
            new_state <= FSMstate_comment3;
        when FSMstate_comment3 =>
            if CODE_DATA = X"23" then
                pcValueInc <= '1';
                new_state <= FSMstate_reset;
            else
                new_state <= FSMstate_comment1;
            end if;
        when FSMstate_numbertohexa =>
            mltpx <= "11";
            pcValueInc <= '1';
            DATA_EN <= '1';
            temporary <= CODE_DATA(3 downto 0) & "0000";
            new_state <= FSMstate_reset;
        when FSMstate_lettertohexa =>
            mltpx <= "11";
            DATA_EN <= '1';
            pcValueInc <= '1';
            temporary <= (CODE_DATA(3 downto 0) + std_logic_vector(conv_unsigned(9, temporary'LENGTH)(3 downto 0))) & "0000";
            new_state <= FSMstate_reset;  
        when FSMstate_return =>
            new_state <= FSMstate_return;
        when FSMstate_others =>
            pcValueInc <= '1';
            new_state <= FSMstate_reset;
        when others =>
    end case;
end process;
end behavioral;