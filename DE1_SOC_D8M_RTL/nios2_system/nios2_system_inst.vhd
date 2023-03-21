	component nios2_system is
		port (
			clk_clk               : in  std_logic                     := 'X';             -- clk
			coords_ram_read_addr  : out std_logic_vector(4 downto 0);                     -- read_addr
			coords_ram_read_data  : in  std_logic_vector(31 downto 0) := (others => 'X'); -- read_data
			coords_ram_write_data : out std_logic_vector(31 downto 0);                    -- write_data
			coords_ram_write_addr : out std_logic_vector(4 downto 0);                     -- write_addr
			esp_uart_rxd          : in  std_logic                     := 'X';             -- rxd
			esp_uart_txd          : out std_logic;                                        -- txd
			reset_reset_n         : in  std_logic                     := 'X'              -- reset_n
		);
	end component nios2_system;

	u0 : component nios2_system
		port map (
			clk_clk               => CONNECTED_TO_clk_clk,               --        clk.clk
			coords_ram_read_addr  => CONNECTED_TO_coords_ram_read_addr,  -- coords_ram.read_addr
			coords_ram_read_data  => CONNECTED_TO_coords_ram_read_data,  --           .read_data
			coords_ram_write_data => CONNECTED_TO_coords_ram_write_data, --           .write_data
			coords_ram_write_addr => CONNECTED_TO_coords_ram_write_addr, --           .write_addr
			esp_uart_rxd          => CONNECTED_TO_esp_uart_rxd,          --   esp_uart.rxd
			esp_uart_txd          => CONNECTED_TO_esp_uart_txd,          --           .txd
			reset_reset_n         => CONNECTED_TO_reset_reset_n          --      reset.reset_n
		);

