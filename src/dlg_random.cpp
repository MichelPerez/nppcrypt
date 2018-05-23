/*
This file is part of the nppcrypt
(http://www.github.com/jeanpaulrichter/nppcrypt)
a plugin for notepad++ [ Copyright (C)2003 Don HO <don.h@free.fr> ]
(https://notepad-plus-plus.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "dlg_random.h"
#include "resource.h"
#include "commctrl.h"
#include "exception.h"
#include "help.h"

DlgRandom::DlgRandom(crypt::Options::Random& opt) : ModalDialog(), options(opt)
{
};

INT_PTR CALLBACK DlgRandom::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
    case WM_INITDIALOG :
	{
		::SendDlgItemMessage(_hSelf, IDC_RANDOM_SPIN, UDM_SETRANGE, true, (LPARAM)MAKELONG(crypt::Constants::rand_char_max, 1));
		::SendDlgItemMessage(_hSelf, IDC_RANDOM_SPIN, UDM_SETBUDDY, (WPARAM)GetDlgItem(_hSelf,IDC_RANDOM_EDIT), 0);		
		if(options.length == 0 || options.length > crypt::Constants::rand_char_max)
			options.length = 16;
		::SendDlgItemMessage(_hSelf, IDC_RANDOM_SPIN, UDM_SETPOS32, 0, options.length);

		switch(options.mode) {
		case crypt::Random::charnum: ::SendDlgItemMessage(_hSelf, IDC_RANDOM_R1, BM_SETCHECK, BST_CHECKED, 0); break;
		case crypt::Random::specials: ::SendDlgItemMessage(_hSelf, IDC_RANDOM_R2, BM_SETCHECK, BST_CHECKED, 0); break;
		case crypt::Random::ascii: ::SendDlgItemMessage(_hSelf, IDC_RANDOM_R3, BM_SETCHECK, BST_CHECKED, 0); break;
		case crypt::Random::base16: ::SendDlgItemMessage(_hSelf, IDC_RANDOM_R4, BM_SETCHECK, BST_CHECKED, 0); break;
		case crypt::Random::base32: ::SendDlgItemMessage(_hSelf, IDC_RANDOM_R5, BM_SETCHECK, BST_CHECKED, 0); break;
		case crypt::Random::base64: ::SendDlgItemMessage(_hSelf, IDC_RANDOM_R6, BM_SETCHECK, BST_CHECKED, 0); break;
		}

		if (!helper::Buffer::isCurrent8Bit()) {
			if (::SendDlgItemMessage(_hSelf, IDC_RANDOM_R3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				::SendDlgItemMessage(_hSelf, IDC_RANDOM_R3, BM_SETCHECK, BST_UNCHECKED, 0);
				::SendDlgItemMessage(_hSelf, IDC_RANDOM_R4, BM_SETCHECK, BST_CHECKED, 0);
			}
		}

		goToCenter();
		return TRUE;
	}
	case WM_COMMAND : 
	{
		if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(_hSelf, IDC_CANCEL);
		}
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			switch (LOWORD(wParam))
			{
			case IDC_OK: case IDC_RANDOM_TOCLIPBOARD:
			{
				try {
					if (!updateOptions()) {
						return FALSE;
					}
					std::basic_string<byte> buffer;
					crypt::random(options, buffer);

					if (LOWORD(wParam) == IDC_OK) {
						helper::Scintilla::replaceSelection(buffer);
					} else {
						helper::Windows::copyToClipboard(buffer);
					}
					EndDialog(_hSelf, IDC_OK);
					return TRUE;
				} catch (CExc& exc) {
					helper::Windows::error(_hSelf, exc.what());
				} catch (...) {
					::MessageBox(_hSelf, TEXT("Unkown Exception!"), TEXT("Error"), MB_OK);
				}
				break;
			}
			}
			break;
		}
		case EN_CHANGE:
		{
			/* prevent out of bounds user input to length spin-control */
			if (LOWORD(wParam) == IDC_RANDOM_EDIT) {
				int temp;
				int len = GetWindowTextLength(::GetDlgItem(_hSelf, IDC_RANDOM_EDIT));
				if (len > 0) {
					std::wstring temp_str;
					temp_str.resize(len + 1);
					::GetDlgItemText(_hSelf, IDC_RANDOM_EDIT, &temp_str[0], (int)temp_str.size());
					temp = std::stoi(temp_str.c_str());
					if (temp > crypt::Constants::rand_char_max) {
						::SendDlgItemMessage(_hSelf, IDC_RANDOM_SPIN, UDM_SETPOS32, 0, crypt::Constants::rand_char_max);
					} else if (temp < 1) {
						::SendDlgItemMessage(_hSelf, IDC_RANDOM_SPIN, UDM_SETPOS32, 0, 1);
					}
				} else {
					::SendDlgItemMessage(_hSelf, IDC_RANDOM_SPIN, UDM_SETPOS32, 0, 1);
				}
			}
			break;
		}
		}
		break;
	} 
	}
	return FALSE;
}

bool DlgRandom::updateOptions()
{
	if (::SendDlgItemMessage(_hSelf, IDC_RANDOM_R1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		options.mode = crypt::Random::charnum;
	} else if (::SendDlgItemMessage(_hSelf, IDC_RANDOM_R2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		options.mode = crypt::Random::specials;
	} else if (::SendDlgItemMessage(_hSelf, IDC_RANDOM_R3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		options.mode = crypt::Random::ascii;
	} else if (::SendDlgItemMessage(_hSelf, IDC_RANDOM_R4, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		options.mode = crypt::Random::base16;
	} else if (::SendDlgItemMessage(_hSelf, IDC_RANDOM_R5, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		options.mode = crypt::Random::base32;
	} else {
		options.mode = crypt::Random::base64;
	}

	options.length = ::SendDlgItemMessage(_hSelf, IDC_RANDOM_SPIN, UDM_GETPOS32, 0, 0);

	// ---------- make sure no binary output for UCS-2 encoding:
	if (!helper::Buffer::isCurrent8Bit() && options.mode == crypt::Random::ascii)
	{
		options.mode = crypt::Random::base16;
		::SendDlgItemMessage(_hSelf, IDC_RANDOM_R3, BM_SETCHECK, BST_UNCHECKED, 0);
		::SendDlgItemMessage(_hSelf, IDC_RANDOM_R4, BM_SETCHECK, BST_CHECKED, 0);
		::MessageBox(_hSelf, TEXT("No binary output for UCS-2 encoding!"), TEXT("Error"), MB_OK);
		return false;
	}
	return true;
}