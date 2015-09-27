/*
This file is part of the NppCrypt Plugin [www.cerberus-design.de] for Notepad++ [ Copyright (C)2003 Don HO <don.h@free.fr> ]

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef DLG_HASH_DEFINE_H
#define DLG_HASH_DEFINE_H

#include "npp/Window.h"
#include "npp/URLCtrl.h"
#include "exception.h"
#include "crypt.h"
#include "unicode.h"

class DlgHash : public Window
{
public:

	DlgHash();
	~DlgHash();
    
    void init(HINSTANCE hInst, HWND parent, crypt::Options::Hash* opt);
    virtual void destroy() { ::DestroyWindow(_hSelf); };
	static DlgHash& Instance() { static DlgHash single; return single; }

   	bool doDialog(bool no_ascii);

private:

	static  BOOL CALLBACK dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	DlgHash(DlgHash const&);
	DlgHash& operator=(DlgHash const&);

	void enableKeyControls(bool v);

	crypt::Options::Hash*	options;
	bool					no_ascii;

	URLCtrl					url_help_hash;
};



#endif
