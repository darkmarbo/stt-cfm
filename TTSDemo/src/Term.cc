//#include "StdAfx.h"
#include "stdafx.h"
#include "Term.h"


Term::Term(void)
{
}


Term::~Term(void)
{
}

Term::Term(string text)
{
	this->text = text;
}

string Term::getText()
{
	return this->text;
}
