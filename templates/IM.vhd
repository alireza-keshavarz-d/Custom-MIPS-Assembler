library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

use IEEE.NUMERIC_STD.ALL;

entity IM is
    Port ( clk : in std_logic;
           ReadAddress : in STD_LOGIC_VECTOR (31 downto 0);
           ReadData : out STD_LOGIC_VECTOR (31 downto 0));
end IM;



architecture Behavioral of IM is

    type IM_type is array (0 to 256) of std_logic_vector (31 downto 0);
    signal IM : IM_type :=(
    ###
    );

begin

    ReadData <= IM(to_integer(Unsigned(ReadAddress(6 downto 0))));

end Behavioral;
