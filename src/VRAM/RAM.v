module RAM ( // This is VRAM, Verilator will generate VRam so don't think it's regular RAM
    input wire clk_gpu,
    input wire clk_vga,

    input wire [19:0] gpu_addr,
    input wire [7:0] gpu_data_in,
    input wire wr_en,

    input wire [19:0] vga_addr,
    output reg [7:0] vga_pixel_out
);
    reg [7:0] vram [0:1048575]; // ~1 MB VRAM

    always @(posedge clk_gpu) begin
        if (wr_en) begin
            vram[gpu_addr] <= gpu_data_in;
        end
    end

    always @(posedge clk_vga) begin
        vga_pixel_out <= vram[vga_addr];
    end

endmodule
