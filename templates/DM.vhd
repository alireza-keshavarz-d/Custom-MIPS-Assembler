library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

use IEEE.NUMERIC_STD.ALL;
entity DM is
    Port ( clk : in std_logic;
           En : in std_logic;
           Address : in STD_LOGIC_VECTOR (31 downto 0);
           WriteData : in STD_LOGIC_VECTOR (31 downto 0);
           ReadData : out STD_LOGIC_VECTOR (31 downto 0));
end DM;

architecture Behavioral of DM is

    type DM_type is array (0 to 256) of std_logic_vector (31 downto 0);
    signal DM : DM_type :=(
    ###
    );

begin

    ReadData <= DM(to_integer(Unsigned(Address(6 downto 0))));
    
    process(clk)
    begin
        if rising_edge(clk) then
            if EN = '1' then
                DM(to_integer(Unsigned(Address(6 downto 0)))) <= WriteData;
            end if;
        end if;
    end process;

end Behavioral;
