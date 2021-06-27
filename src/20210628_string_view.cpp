﻿/*
 * std::string_view
 */

#include <string_view>

#include <iostream>
#include <memory>

char const code[] = R"(
	ORG $9000
VERSN	DFB $02
HIMEM	LDA #$FF
	STA $4C
	LDA #$8F
	STA $4D
DOPTCH	LDA #$20
	STA $A180
	LDA #$5B
	STA $A181
	LDA #$A7
	STA $A182
RUNPTCH	LDA #$AD
	STA $A4D1
	LDA #$B6
	STA $A4D2
	LDA #$AA
	STA $A4D3
LODPTCH	LDA #$4C
	STA $A413
	LDA #>LOD
	STA $A414
	LDA #<LOD
	STA $A415
BLDPTCH	LDA #$4C
	STA $A35D
	LDA #>BLOD
	STA $A35E
	LDA #<BLOD
	STA $A35F
CATPTCH	LDA #$4C
	STA $A56E
	LDA #>CATALOG
	STA $A56F
	LDA #<CATALOG
	STA $A570
USRPTCH	LDA #$4C
	STA $0A
	LDA #>USRCMD
	STA $0B
	LDA #<USRCMD
	STA $0C
	...
)";

int main()
{
	std::cout << "code @ " << static_cast<void const*>(code) << std::endl;

	// This will make a copy of code into pc1.
	std::string pc1{code};
	std::cout << "pc1 @ " << static_cast<void*>(pc1.data()) << std::endl;

	// This will create another copy. Copy-on-write is not allowed anymore
	// since C++11 (which was the default implementation by gcc).
	auto pc2 = std::make_unique<std::string>(pc1);
	std::cout << "pc2 @ " << static_cast<void*>(pc2->data()) << std::endl;

	// This will create yet another copy (of the start of code).  That is
	// nice, if code, pc1 or pc2 would be changed, but if you know they
	// will not, it is a waste of memory and time.
	auto header = pc2->substr(0, pc2->find("HIMEM"));
	std::cout << "header @ " << static_cast<void*>(header.data()) << std::endl;

	// std::string_view is just a pointer/length pair to another block of
	// memory.  It does not contain the string itself. So, it is cheap to
	// copy. It's interface is basically all std::string's functions, that
	// do not modify the string.
	std::string_view load{code};
	std::cout << "load @ " << static_cast<void const*>(load.data()) << std::endl;

	// So, this will return a new std::string_view, but still pointing to code.
	auto scan = load.substr(0, load.find("HIMEM"));
	std::cout << "scan @ " << static_cast<void const*>(scan.data()) << std::endl;

	// You can have a view on an std::string...
	std::string_view uhoh = *pc2;
	// ...but you are responsible for making sure that the data the view
	// points to remains valid.
	std::cout << "uhoh @ " << static_cast<void const*>(uhoh.data()) << std::endl;

	*pc2 = "McAfee";
	std::cout << uhoh.data() << std::endl;
	pc2.reset();

	// uhoh points now to inaccessible data. You can try if your sanitizer
	// kicks in...
//	std::cout << uhoh.data() << std::endl;
}

