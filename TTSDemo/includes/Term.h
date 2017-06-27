#pragma once

#include "stdafx.h"

class Term
{
	string text;

public:
	Term(void);
	Term(string text);
	~Term(void);
	string getText();
};

