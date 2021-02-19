#include "far/plugin.hpp"
#include "common.h"

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

static const wchar_t* TITLE = L"IndentChecker";
static const wchar_t* DESCRIPTION = L"Checks indentation. Colors tabs if both spaces and tabs are "
	"used.";
static const VersionInfo PLUGIN_VERSION = {1, 0, 0, 1, VS_RELEASE};

static const unsigned int PLUGIN_COLORING_PRIORITY = 2;
static const FarColor PLUGIN_SPACE_COLOR = {FCF_4BITMASK, {0}, {4}, {0, 0}};
static const FarColor PLUGIN_TAB_COLOR = {FCF_4BITMASK, {0}, {14}, {0, 0}};

enum TypeOfIndentation {
	TABS_INDENTATION,
	SPACES_INDENTATION,
	MIXED_INDENTATION
};
typedef enum TypeOfIndentation TypeOfIndentation;

bool isFirstLine;
static TypeOfIndentation lastTypeOfIndentation;
static RegisterValue lastLineWithIndentation;
static void newColoring() {
	isFirstLine = true;
}

static bool isSpaceCharacter(const wchar_t character) {
	return character == L' ' || character == L'\t';
}

void colorLeadingSpaceCharacters(RegisterValue lineNumber) {
	const wchar_t* line;
	RegisterValue lineLength = 0;
	getLine(lineNumber, &line, &lineLength);
	assert(lineLength != 0);
	EXPLICITLY_UNUSED(lineLength);

	for (RegisterValue pos = 0; pos < lineNumber; ++pos) {
		if (line[pos] == L' ') {
			colorCharacter(lineNumber, pos, PLUGIN_SPACE_COLOR, ECF_NONE);
		} else if (line[pos] == L'\t') {
			colorCharacter(lineNumber, pos, PLUGIN_TAB_COLOR, ECF_NONE);
		} else {
			break;
		}
	}
}

static void colorLine(const RegisterValue lineNumber) {
	const wchar_t* line;
	RegisterValue lineLength;
	getLine(lineNumber, &line, &lineLength);
	if (lineLength == 0) {
		return;
	}
	
	RegisterValue beginPos = 0;
	RegisterValue endPos = beginPos;
	for (; endPos + 1 < lineLength && isSpaceCharacter(line[endPos + 1]); ++endPos);
	
	if (endPos + 1 == lineLength) {
		return;
	}
	
	bool containsSpaces = false;
	for (RegisterValue pos = beginPos; pos <= endPos; ++pos) {
		if (line[pos] == L' ') {
			containsSpaces = true;
			break;
		}
	}
	
	bool containsTabs = false;
	for (RegisterValue pos = beginPos; pos <= endPos; ++pos) {
		if (line[pos] == L'\t') {
			containsTabs = true;
			break;
		}
	}
	
	TypeOfIndentation typeOfIndentation;
	if (!containsSpaces) {
		// Indentation is tabs as the indentation is not empty.
		typeOfIndentation = TABS_INDENTATION;
	} else if (!containsTabs) {
		// Indentation is spaces as the indentation is not empty.
		typeOfIndentation = SPACES_INDENTATION;
	} else {
		typeOfIndentation = MIXED_INDENTATION;
	}
	if (typeOfIndentation == MIXED_INDENTATION) {
		colorLeadingSpaceCharacters(lineNumber);
	} else if (
		!isFirstLine &&
		lastTypeOfIndentation != MIXED_INDENTATION &&
		typeOfIndentation != lastTypeOfIndentation
	) {
		colorLeadingSpaceCharacters(lastLineWithIndentation);
		colorLeadingSpaceCharacters(lineNumber);
	}
	lastTypeOfIndentation = typeOfIndentation;
	isFirstLine = false;
	lastLineWithIndentation = lineNumber;
}