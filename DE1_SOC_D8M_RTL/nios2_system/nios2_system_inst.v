	nios2_system u0 (
		.clk_clk               (<connected-to-clk_clk>),               //        clk.clk
		.coords_ram_read_addr  (<connected-to-coords_ram_read_addr>),  // coords_ram.read_addr
		.coords_ram_read_data  (<connected-to-coords_ram_read_data>),  //           .read_data
		.coords_ram_write_addr (<connected-to-coords_ram_write_addr>), //           .write_addr
		.coords_ram_write_en   (<connected-to-coords_ram_write_en>),   //           .write_en
		.coords_ram_write_data (<connected-to-coords_ram_write_data>), //           .write_data
		.esp_uart_rxd          (<connected-to-esp_uart_rxd>),          //   esp_uart.rxd
		.esp_uart_txd          (<connected-to-esp_uart_txd>),          //           .txd
		.reset_reset_n         (<connected-to-reset_reset_n>)          //      reset.reset_n
	);

