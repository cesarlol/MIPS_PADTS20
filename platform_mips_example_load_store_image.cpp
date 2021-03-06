#include "platform_mips.h"
#include "opencv.h"

using namespace cv;

	Mat SRC;
	Mat DST;

platform_mips::platform_mips(const char* elf_file, const char* dmp_file, bool verbose) : mips32(verbose)
{
	ereader.read_elf(elf_file);
	ereader.print_elf(dmp_file);
	memory = ereader.mem;
	vb = verbose;
}

void platform_mips::run_mips(void)
{
	while (get_status(1) == false) {
		uint32_t idata = fetch();
		MIPS32::instr_t instr = decode(idata);
		execute(instr);
	}
}

int32_t platform_mips::LoadMemory(MIPS32::word_size_t word_size, uint32_t paddr, uint32_t vaddr, MIPS32::mem_type_t mem_type)
{
	uint32_t mem;
	uint8_t* mem_byte = (uint8_t*)(&mem);

	if (paddr = IMAGE_SRC_OFFSET) {
		// Read Image
		SRC = imread("src", CV_LOAD_IMAGE_GRAYSCALE); //8 bit, no of channels = 1 or use parameter CV_8UC1
		if( SRC.empty()){
			printf("Error: Can not load image :-)\n");
			return -1;
		}
	}
	else {
		mem_byte[0] = memory[(paddr & 0xFFFFFFFC) + 3];
		mem_byte[1] = memory[(paddr & 0xFFFFFFFC) + 2];
		mem_byte[2] = memory[(paddr & 0xFFFFFFFC) + 1];
		mem_byte[3] = memory[(paddr & 0xFFFFFFFC) + 0];
	}

	return mem;
}

void platform_mips::StoreMemory(MIPS32::word_size_t word_size, int32_t data, uint32_t paddr, uint32_t vaddr, MIPS32::mem_type_t mem_type)
{
	uint32_t mem;
	uint8_t* data_byte = (uint8_t*)(&data);

	if (paddr = IMAGE_DST_OFFSET) {
		// Store Image (Borders detected)
		imwrite("dst.jpg", DST);
	}
	else {
		memory[(paddr & 0xFFFFFFFC) + 0] = data_byte[3];
		memory[(paddr & 0xFFFFFFFC) + 1] = data_byte[2];
		memory[(paddr & 0xFFFFFFFC) + 2] = data_byte[1];
		memory[(paddr & 0xFFFFFFFC) + 3] = data_byte[0];
	}
}

void platform_mips::print_stats(const char* stats_file)
{
	FILE* sfile;
	sfile = fopen(stats_file, "a");

	fprintf(sfile, "\n*** MIPS32 Statistics ***\n");
	fprintf(sfile, "Arithmetic Instructions:\t%d\n", cntr.arithmetic);
	fprintf(sfile, "Control Instructions:\t\t%d\n", cntr.control);
	fprintf(sfile, "Jump Instructions:\t\t\t%d\n", cntr.jump);
	fprintf(sfile, "Logical Instructions:\t\t%d\n", cntr.logical);
	fprintf(sfile, "Memory Instructions:\t\t%d\n", cntr.memory);
	fprintf(sfile, "Move Instructions:\t\t\t%d\n", cntr.move);
	fprintf(sfile, "Privileged Instructions:\t%d\n", cntr.privileged);
	fprintf(sfile, "Shift Instructions:\t\t\t%d\n", cntr.shift);
	fprintf(sfile, "Trap Instructions:\t\t\t%d\n", cntr.trap);
	fprintf(sfile, "\nTotal Instructions:\t\t\t%d\n", (cntr.arithmetic + cntr.control + cntr.jump + cntr.logical + cntr.memory + cntr.move + cntr.privileged + cntr.shift + cntr.trap));

	fclose(sfile);
}
