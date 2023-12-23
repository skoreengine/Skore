// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Core/StringView.hpp"
#include "Skore/Math/MathTypes.hpp"
namespace Skore
{
	struct Window;

	enum Key_
	{
		Key_Space = 32,
		Key_Apostrophe = 39,
		Key_Comma = 44,
		Key_Minus = 45,
		Key_Period = 46,
		Key_Slash = 47,
		Key_Num0 = 48,
		Key_Num1 = 49,
		Key_Num2 = 50,
		Key_Num3 = 51,
		Key_Num4 = 52,
		Key_Num5 = 53,
		Key_Num6 = 54,
		Key_Num7 = 55,
		Key_Num8 = 56,
		Key_Num9 = 57,
		Key_Semicolon = 59,
		Key_Equal = 61,
		Key_A = 65,
		Key_B = 66,
		Key_C = 67,
		Key_D = 68,
		Key_E = 69,
		Key_F = 70,
		Key_G = 71,
		Key_H = 72,
		Key_I = 73,
		Key_J = 74,
		Key_K = 75,
		Key_L = 76,
		Key_M = 77,
		Key_N = 78,
		Key_O = 79,
		Key_P = 80,
		Key_Q = 81,
		Key_R = 82,
		Key_S = 83,
		Key_T = 84,
		Key_U = 85,
		Key_V = 86,
		Key_W = 87,
		Key_X = 88,
		Key_Y = 89,
		Key_Z = 90,
		Key_LeftBracket = 91,
		Key_Backslash = 92,
		Key_RightBracket = 93,
		Key_GraveAccent = 96,
		Key_World1 = 161,
		Key_World2 = 162,
		Key_Escape = 256,
		Key_Enter = 257,
		Key_Tab = 258,
		Key_Backspace = 259,
		Key_Insert = 260,
		Key_Delete = 261,
		Key_Right = 262,
		Key_Left = 263,
		Key_Down = 264,
		Key_Up = 265,
		Key_PageUp = 266,
		Key_PageDown = 267,
		Key_Home = 268,
		Key_End = 269,
		Key_CapsLock = 280,
		Key_ScrollLock = 281,
		Key_NumLock = 282,
		Key_PrintScreen = 283,
		Key_Pause = 284,
		Key_F1 = 290,
		Key_F2 = 291,
		Key_F3 = 292,
		Key_F4 = 293,
		Key_F5 = 294,
		Key_F6 = 295,
		Key_F7 = 296,
		Key_F8 = 297,
		Key_F9 = 298,
		Key_F10 = 299,
		Key_F11 = 300,
		Key_F12 = 301,
		Key_F13 = 302,
		Key_F14 = 303,
		Key_F15 = 304,
		Key_F16 = 305,
		Key_F17 = 306,
		Key_F18 = 307,
		Key_F19 = 308,
		Key_F20 = 309,
		Key_F21 = 310,
		Key_F22 = 311,
		Key_F23 = 312,
		Key_F24 = 313,
		Key_F25 = 314,
		Key_Keypad0 = 320,
		Key_Keypad1 = 321,
		Key_Keypad2 = 322,
		Key_Keypad3 = 323,
		Key_Keypad4 = 324,
		Key_Keypad5 = 325,
		Key_Keypad6 = 326,
		Key_Keypad7 = 327,
		Key_Keypad8 = 328,
		Key_Keypad9 = 329,
		Key_KeypadDecimal = 330,
		Key_KeypadDivide = 331,
		Key_KeypadMultiply = 332,
		Key_KeypadSubtract = 333,
		Key_KeypadAdd = 334,
		Key_KeypadEnter = 335,
		Key_KeypadEqual = 336,
		Key_LeftShift = 340,
		Key_LeftCtrl = 341,
		Key_LeftAlt = 342,
		Key_LeftSuper = 343,
		Key_RightShift = 344,
		Key_RightCtrl = 345,
		Key_RightAlt = 346,
		Key_RightSuper = 347,
		Key_Menu = 348
	};

	enum MouseCursor_
	{
		MouseCursor_None = -1,
		MouseCursor_Arrow = 0,
		MouseCursor_TextInput = 1,
		MouseCursor_ResizeAll = 2,
		MouseCursor_ResizeNS = 3,
		MouseCursor_ResizeWE = 4,
		MouseCursor_ResizeNESW = 5,
		MouseCursor_ResizeNWSE = 6,
		MouseCursor_Hand = 7,
		MouseCursor_NotAllowed = 8,
		MouseCursor_COUT
	};

	enum MouseButton_
	{
		MouseButton_Left = 0,
		MouseButton_Right = 1,
		MouseButton_Middle = 2,
		MouseButton_None = 999
	};

	enum InputAction_
	{
		InputAction_Release = 0,
		InputAction_Press = 1,
	};

	enum MessageType
	{
		MessageType_Info        = 0,
		MessageType_Warning     = 1,
		MessageType_Error       = 2
	};

	enum DialogResult
	{
		DialogResult_Error,
		DialogResult_Okay,
		DialogResult_Cancel
	};

	enum WindowFlags_
	{
		WindowFlags_None      = 0,
		WindowFlags_Maximized = 1 << 0
	};

	using WindowFlags = i32;

	struct FileFilter
	{
		const char* name{};
		const char* spec{};
	};

	struct WindowCreation
	{
		StringView  Title       = "";
		Rect        Rect        = {0,0,800, 600};
		WindowFlags WindowFlags = WindowFlags_None;
	};
}