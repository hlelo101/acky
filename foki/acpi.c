#include "acpi.h"
#include "memory.h"
#include "serial.h"
#include "herr.h"

RSDP_Desc rsdp  = {0};
RSDT *rsdt      = {0};
FADT *fadt      = {0};
DSDT *dsdt      = {0};

uint8_t decodePkgLength(uint8_t *ptr, uint32_t *len) {
    uint8_t lead = ptr[0];
    uint8_t byteCount = (lead >> 6) & 0x03; // Number of addditional bytes
    uint32_t length = lead & 0x3F; // Lower 6 bits

    for (uint8_t i = 1; i <= byteCount; i++) {
        length |= ((uint32_t)ptr[i]) << (4 + (8 * (i - 1)));
    }

    *len = length;
    return byteCount + 1; // total bytes used
}

// Why the fuck is x86 that complex
uint8_t getSLP_TYPa() {
    if(!dsdt) return 0;
    uint8_t *aml = dsdt->aml;
    uint32_t len = dsdt->header.length;

    for(uint32_t i = 0; i < len - 6; i++) {
        if(aml[i] == '_' && aml[i+1] == 'S' && aml[i+2] == '5' && aml[i+3] == '_') { // NameOp + _S5_
            if(aml[i + 4] == 0x12) {
                uint32_t pkgLen = 0;
                uint32_t pkgLenBytes = decodePkgLength(&aml[i + 5], &pkgLen);

                return aml[i + 5 + pkgLenBytes];
            }
        }
    }

    return 0;
}

// Something worth noting: rsdp.revision is 0 if ACPI 1.0 is used, and 2 if ACPI 2.0 is used.
// 2 will also be used if ACPI version higher than 2 is used.
void initACPI() {
    // Read EBDA segment pointer from 0x40E
    uint32_t ebdaSegment = 0;
    memcpy(&ebdaSegment, (void *)(uint16_t)0x40E, sizeof(uint16_t)); // The reason why we need a whole call to memcpy is to make GCC stfu
    ebdaSegment <<= 4;

    bool foundRSDP = false;
    for (uint32_t addr = ebdaSegment; addr < ebdaSegment + 1024; addr++) { // We should be able to increment it by 16 instead of 1, but... idk, already works fine
        if (memcmp((void *)addr, "RSD PTR ", 8) == 0) {
            memcpy(&rsdp, (void *)addr, sizeof(RSDP_Desc));
            foundRSDP = true;
            serialSendString("[ACPI]: Found RSDP at: "); serialSendInt(addr); serialSendString("\n");
            break;
        }
    }
    // Search from 0x000E0000 to 0x000FFFFF if not found in EBDA
    if(!foundRSDP) {
        for (uint32_t addr = 0xE0000; addr < 0xFFFFF; addr++) { // Same remark as above regarding that loop...
            if (memcmp((void *)addr, "RSD PTR ", 8) == 0) {
                memcpy(&rsdp, (void *)addr, sizeof(RSDP_Desc));
                foundRSDP = true;
                serialSendString("[ACPI]: Found RSDP at: "); serialSendInt(addr); serialSendString("\n");
                break;
            }
        }
    }
    if(!foundRSDP) herr("ACPI RSDP not found");

    // We now have to check if the RSDP is valid
    uint8_t *bytes = (uint8_t *)&rsdp;
    uint8_t sum = 0;
    for(uint32_t i = 0; i < sizeof(RSDP_Desc); i++) {
        sum += bytes[i];
    }
    if(sum != 0) herr("ACPI RSDP checksum failed");

    // We should techincally use the XSTD if we detect that the version is >= 2, but who cares
    rsdt = (RSDT *)rsdp.rsdtAddress;
    if(memcmp(rsdt->header.signature, "RSDT", 4) != 0 && memcmp(rsdt->header.signature, "XSDT", 4) != 0) herr("ACPI RSDT/XSDT signature mismatch");

    // Find the FADT, since it's really the only thing we care about
    int numEntries = (rsdt->header.length - sizeof(STDHeader)) / 4;
    for(int i = 0; i < numEntries; i++) {
        STDHeader *h = (STDHeader *)rsdt->STRp[i];
        if(memcmp(h->signature, "FACP", 4) == 0) {
            fadt = (FADT *)h;
            dsdt = (DSDT *)fadt->Dsdt;
            serialSendString("[ACPI]: Found DSDT at: "); serialSendInt((uint32_t)dsdt); serialSendString("\n");
            break;
        }
    }

    // Enable ACPI
    outb(fadt->SMI_CommandPort, fadt->AcpiEnable);
}

void shutdown() {
    if(!fadt) return; // Fuck off
    asm volatile("cli");
    outw(fadt->PM1aControlBlock, getSLP_TYPa() | 0x2000);
    while(1);
}

// This technically has nothing to do with ACPI but it's still related
// to powering on/off/rebooting the computer and I won't make a whole
// new file just for it...
void reboot() {
    uint8_t good = 0x02;
    while (good & 0x02) good = inb(0x64);
    outb(0x64, 0xFE);
    while(1) asm volatile("hlt");
}
