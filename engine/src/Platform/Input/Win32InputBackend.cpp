// ============================================================================
// Win32InputBackend.cpp
// ----------------------------------------------------------------------------
// Windows implementation of IInputBackend.
// Translates WM_* messages to engine input events.
//
// ============================================================================

#include "Win32InputBackend.h"

#include <windowsx.h>  // GET_X_LPARAM, GET_Y_LPARAM

// ============================================================================
// VK_* to Key Translation Table
// ============================================================================

static constexpr Key s_VirtualKeyToKey[] = {
    Key::Unknown,         // 0x00
    Key::Unknown,         // 0x01 VK_LBUTTON (handled separately)
    Key::Unknown,         // 0x02 VK_RBUTTON
    Key::Unknown,         // 0x03 VK_CANCEL
    Key::Unknown,         // 0x04 VK_MBUTTON
    Key::Unknown,         // 0x05 VK_XBUTTON1
    Key::Unknown,         // 0x06 VK_XBUTTON2
    Key::Unknown,         // 0x07
    Key::Backspace,       // 0x08 VK_BACK
    Key::Tab,             // 0x09 VK_TAB
    Key::Unknown,         // 0x0A
    Key::Unknown,         // 0x0B
    Key::Unknown,         // 0x0C VK_CLEAR
    Key::Enter,           // 0x0D VK_RETURN
    Key::Unknown,         // 0x0E
    Key::Unknown,         // 0x0F
    Key::LeftShift,       // 0x10 VK_SHIFT (generic, refined below)
    Key::LeftCtrl,        // 0x11 VK_CONTROL (generic, refined below)
    Key::LeftAlt,         // 0x12 VK_MENU (generic, refined below)
    Key::Pause,           // 0x13 VK_PAUSE
    Key::CapsLock,        // 0x14 VK_CAPITAL
    Key::Unknown,         // 0x15 VK_KANA
    Key::Unknown,         // 0x16
    Key::Unknown,         // 0x17 VK_JUNJA
    Key::Unknown,         // 0x18 VK_FINAL
    Key::Unknown,         // 0x19 VK_KANJI
    Key::Unknown,         // 0x1A
    Key::Escape,          // 0x1B VK_ESCAPE
    Key::Unknown,         // 0x1C VK_CONVERT
    Key::Unknown,         // 0x1D VK_NONCONVERT
    Key::Unknown,         // 0x1E VK_ACCEPT
    Key::Unknown,         // 0x1F VK_MODECHANGE
    Key::Space,           // 0x20 VK_SPACE
    Key::PageUp,          // 0x21 VK_PRIOR
    Key::PageDown,        // 0x22 VK_NEXT
    Key::End,             // 0x23 VK_END
    Key::Home,            // 0x24 VK_HOME
    Key::Left,            // 0x25 VK_LEFT
    Key::Up,              // 0x26 VK_UP
    Key::Right,           // 0x27 VK_RIGHT
    Key::Down,            // 0x28 VK_DOWN
    Key::Unknown,         // 0x29 VK_SELECT
    Key::Unknown,         // 0x2A VK_PRINT
    Key::Unknown,         // 0x2B VK_EXECUTE
    Key::PrintScreen,     // 0x2C VK_SNAPSHOT
    Key::Insert,          // 0x2D VK_INSERT
    Key::Delete,          // 0x2E VK_DELETE
    Key::Unknown,         // 0x2F VK_HELP
    Key::Num0,            // 0x30 '0'
    Key::Num1,            // 0x31 '1'
    Key::Num2,            // 0x32 '2'
    Key::Num3,            // 0x33 '3'
    Key::Num4,            // 0x34 '4'
    Key::Num5,            // 0x35 '5'
    Key::Num6,            // 0x36 '6'
    Key::Num7,            // 0x37 '7'
    Key::Num8,            // 0x38 '8'
    Key::Num9,            // 0x39 '9'
    Key::Unknown,         // 0x3A
    Key::Unknown,         // 0x3B
    Key::Unknown,         // 0x3C
    Key::Unknown,         // 0x3D
    Key::Unknown,         // 0x3E
    Key::Unknown,         // 0x3F
    Key::Unknown,         // 0x40
    Key::A,               // 0x41 'A'
    Key::B,               // 0x42 'B'
    Key::C,               // 0x43 'C'
    Key::D,               // 0x44 'D'
    Key::E,               // 0x45 'E'
    Key::F,               // 0x46 'F'
    Key::G,               // 0x47 'G'
    Key::H,               // 0x48 'H'
    Key::I,               // 0x49 'I'
    Key::J,               // 0x4A 'J'
    Key::K,               // 0x4B 'K'
    Key::L,               // 0x4C 'L'
    Key::M,               // 0x4D 'M'
    Key::N,               // 0x4E 'N'
    Key::O,               // 0x4F 'O'
    Key::P,               // 0x50 'P'
    Key::Q,               // 0x51 'Q'
    Key::R,               // 0x52 'R'
    Key::S,               // 0x53 'S'
    Key::T,               // 0x54 'T'
    Key::U,               // 0x55 'U'
    Key::V,               // 0x56 'V'
    Key::W,               // 0x57 'W'
    Key::X,               // 0x58 'X'
    Key::Y,               // 0x59 'Y'
    Key::Z,               // 0x5A 'Z'
    Key::LeftSuper,       // 0x5B VK_LWIN
    Key::RightSuper,      // 0x5C VK_RWIN
    Key::Unknown,         // 0x5D VK_APPS
    Key::Unknown,         // 0x5E
    Key::Unknown,         // 0x5F VK_SLEEP
    Key::Numpad0,         // 0x60 VK_NUMPAD0
    Key::Numpad1,         // 0x61 VK_NUMPAD1
    Key::Numpad2,         // 0x62 VK_NUMPAD2
    Key::Numpad3,         // 0x63 VK_NUMPAD3
    Key::Numpad4,         // 0x64 VK_NUMPAD4
    Key::Numpad5,         // 0x65 VK_NUMPAD5
    Key::Numpad6,         // 0x66 VK_NUMPAD6
    Key::Numpad7,         // 0x67 VK_NUMPAD7
    Key::Numpad8,         // 0x68 VK_NUMPAD8
    Key::Numpad9,         // 0x69 VK_NUMPAD9
    Key::NumpadMultiply,  // 0x6A VK_MULTIPLY
    Key::NumpadAdd,       // 0x6B VK_ADD
    Key::Unknown,         // 0x6C VK_SEPARATOR
    Key::NumpadSubtract,  // 0x6D VK_SUBTRACT
    Key::NumpadDecimal,   // 0x6E VK_DECIMAL
    Key::NumpadDivide,    // 0x6F VK_DIVIDE
    Key::F1,              // 0x70 VK_F1
    Key::F2,              // 0x71 VK_F2
    Key::F3,              // 0x72 VK_F3
    Key::F4,              // 0x73 VK_F4
    Key::F5,              // 0x74 VK_F5
    Key::F6,              // 0x75 VK_F6
    Key::F7,              // 0x76 VK_F7
    Key::F8,              // 0x77 VK_F8
    Key::F9,              // 0x78 VK_F9
    Key::F10,             // 0x79 VK_F10
    Key::F11,             // 0x7A VK_F11
    Key::F12,             // 0x7B VK_F12
};

static constexpr size_t s_VirtualKeyTableSize = sizeof(s_VirtualKeyToKey) / sizeof(s_VirtualKeyToKey[0]);

// ============================================================================
// TranslateVirtualKey
// ============================================================================

Key Win32InputBackend::TranslateVirtualKey(WPARAM VirtualKey) noexcept
{
	// Handle extended keys and left/right variants
	if (VirtualKey == VK_SHIFT)
	{
		// Check actual key via GetKeyState
		if (GetKeyState(VK_LSHIFT) & 0x8000)
			return Key::LeftShift;
		if (GetKeyState(VK_RSHIFT) & 0x8000)
			return Key::RightShift;
		return Key::LeftShift;
	}
	if (VirtualKey == VK_CONTROL)
	{
		if (GetKeyState(VK_LCONTROL) & 0x8000)
			return Key::LeftCtrl;
		if (GetKeyState(VK_RCONTROL) & 0x8000)
			return Key::RightCtrl;
		return Key::LeftCtrl;
	}
	if (VirtualKey == VK_MENU)
	{
		if (GetKeyState(VK_LMENU) & 0x8000)
			return Key::LeftAlt;
		if (GetKeyState(VK_RMENU) & 0x8000)
			return Key::RightAlt;
		return Key::LeftAlt;
	}

	// Explicit left/right variants
	if (VirtualKey == VK_LSHIFT)
		return Key::LeftShift;
	if (VirtualKey == VK_RSHIFT)
		return Key::RightShift;
	if (VirtualKey == VK_LCONTROL)
		return Key::LeftCtrl;
	if (VirtualKey == VK_RCONTROL)
		return Key::RightCtrl;
	if (VirtualKey == VK_LMENU)
		return Key::LeftAlt;
	if (VirtualKey == VK_RMENU)
		return Key::RightAlt;

	// Numpad Enter (extended key)
	if (VirtualKey == VK_RETURN)
	{
		// Note: Extended bit check would need lParam, handled in ProcessMessage
		return Key::Enter;
	}

	// Table lookup
	if (VirtualKey < s_VirtualKeyTableSize)
	{
		return s_VirtualKeyToKey[VirtualKey];
	}

	// OEM keys (keyboard layout dependent)
	switch (VirtualKey)
	{
		case VK_OEM_1:
			return Key::Semicolon;  // ;:
		case VK_OEM_PLUS:
			return Key::Equals;  // =+
		case VK_OEM_COMMA:
			return Key::Comma;  // ,<
		case VK_OEM_MINUS:
			return Key::Minus;  // -_
		case VK_OEM_PERIOD:
			return Key::Period;  // .>
		case VK_OEM_2:
			return Key::Slash;  // /?
		case VK_OEM_3:
			return Key::Grave;  // `~
		case VK_OEM_4:
			return Key::LeftBracket;  // [{
		case VK_OEM_5:
			return Key::Backslash;  // \|
		case VK_OEM_6:
			return Key::RightBracket;  // ]}
		case VK_OEM_7:
			return Key::Apostrophe;  // '"
		case VK_NUMLOCK:
			return Key::NumLock;
		case VK_SCROLL:
			return Key::ScrollLock;
		default:
			return Key::Unknown;
	}
}

// ============================================================================
// GetCurrentModifiers
// ============================================================================

ModifierFlags Win32InputBackend::GetCurrentModifiers() noexcept
{
	ModifierFlags Flags = ModifierFlags::None;

	if (GetKeyState(VK_LSHIFT) & 0x8000)
		Flags = Flags | ModifierFlags::LeftShift;
	if (GetKeyState(VK_RSHIFT) & 0x8000)
		Flags = Flags | ModifierFlags::RightShift;
	if (GetKeyState(VK_LCONTROL) & 0x8000)
		Flags = Flags | ModifierFlags::LeftCtrl;
	if (GetKeyState(VK_RCONTROL) & 0x8000)
		Flags = Flags | ModifierFlags::RightCtrl;
	if (GetKeyState(VK_LMENU) & 0x8000)
		Flags = Flags | ModifierFlags::LeftAlt;
	if (GetKeyState(VK_RMENU) & 0x8000)
		Flags = Flags | ModifierFlags::RightAlt;
	if (GetKeyState(VK_CAPITAL) & 0x0001)
		Flags = Flags | ModifierFlags::CapsLock;
	if (GetKeyState(VK_NUMLOCK) & 0x0001)
		Flags = Flags | ModifierFlags::NumLock;

	return Flags;
}

// ============================================================================
// ProcessMessage
// ============================================================================

InputBackendResult Win32InputBackend::ProcessMessage(uint32_t Msg, uintptr_t Param1, intptr_t Param2)
{
	InputBackendResult Result;
	WPARAM WParam = static_cast<WPARAM>(Param1);
	LPARAM LParam = static_cast<LPARAM>(Param2);

	switch (Msg)
	{
		// -------------------------------------------------------------------------
		// Keyboard
		// -------------------------------------------------------------------------
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			Result.Type = InputEventType::Keyboard;
			Result.Keyboard.KeyCode = TranslateVirtualKey(WParam);
			Result.Keyboard.bPressed = true;
			Result.Keyboard.bRepeat = (LParam & 0x40000000) != 0;  // Bit 30 = previous state
			Result.Keyboard.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			Result.Type = InputEventType::Keyboard;
			Result.Keyboard.KeyCode = TranslateVirtualKey(WParam);
			Result.Keyboard.bPressed = false;
			Result.Keyboard.bRepeat = false;
			Result.Keyboard.Modifiers = GetCurrentModifiers();
			break;
		}

		// -------------------------------------------------------------------------
		// Mouse Buttons
		// -------------------------------------------------------------------------
		case WM_LBUTTONDOWN:
		{
			Result.Type = InputEventType::MouseButton;
			Result.MouseButton.Button = MouseButton::Left;
			Result.MouseButton.bPressed = true;
			Result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			Result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			Result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_LBUTTONUP:
		{
			Result.Type = InputEventType::MouseButton;
			Result.MouseButton.Button = MouseButton::Left;
			Result.MouseButton.bPressed = false;
			Result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			Result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			Result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_RBUTTONDOWN:
		{
			Result.Type = InputEventType::MouseButton;
			Result.MouseButton.Button = MouseButton::Right;
			Result.MouseButton.bPressed = true;
			Result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			Result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			Result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_RBUTTONUP:
		{
			Result.Type = InputEventType::MouseButton;
			Result.MouseButton.Button = MouseButton::Right;
			Result.MouseButton.bPressed = false;
			Result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			Result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			Result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_MBUTTONDOWN:
		{
			Result.Type = InputEventType::MouseButton;
			Result.MouseButton.Button = MouseButton::Middle;
			Result.MouseButton.bPressed = true;
			Result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			Result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			Result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_MBUTTONUP:
		{
			Result.Type = InputEventType::MouseButton;
			Result.MouseButton.Button = MouseButton::Middle;
			Result.MouseButton.bPressed = false;
			Result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			Result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			Result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_XBUTTONDOWN:
		{
			Result.Type = InputEventType::MouseButton;
			Result.MouseButton.Button = (GET_XBUTTON_WPARAM(WParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;
			Result.MouseButton.bPressed = true;
			Result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			Result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			Result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		case WM_XBUTTONUP:
		{
			Result.Type = InputEventType::MouseButton;
			Result.MouseButton.Button = (GET_XBUTTON_WPARAM(WParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;
			Result.MouseButton.bPressed = false;
			Result.MouseButton.Position.X = GET_X_LPARAM(LParam);
			Result.MouseButton.Position.Y = GET_Y_LPARAM(LParam);
			Result.MouseButton.Modifiers = GetCurrentModifiers();
			break;
		}

		// -------------------------------------------------------------------------
		// Mouse Move
		// -------------------------------------------------------------------------
		case WM_MOUSEMOVE:
		{
			Result.Type = InputEventType::MouseMove;
			Result.MouseMove.Position.X = GET_X_LPARAM(LParam);
			Result.MouseMove.Position.Y = GET_Y_LPARAM(LParam);
			// Delta calculated by InputSystem (needs previous position)
			Result.MouseMove.Delta.X = 0;
			Result.MouseMove.Delta.Y = 0;
			break;
		}

		// -------------------------------------------------------------------------
		// Mouse Wheel
		// -------------------------------------------------------------------------
		case WM_MOUSEWHEEL:
		{
			Result.Type = InputEventType::MouseWheel;
			Result.MouseWheel.Delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(WParam)) / WHEEL_DELTA;
			Result.MouseWheel.bHorizontal = false;
			Result.MouseWheel.Position.X = GET_X_LPARAM(LParam);
			Result.MouseWheel.Position.Y = GET_Y_LPARAM(LParam);
			break;
		}

		case WM_MOUSEHWHEEL:
		{
			Result.Type = InputEventType::MouseWheel;
			Result.MouseWheel.Delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(WParam)) / WHEEL_DELTA;
			Result.MouseWheel.bHorizontal = true;
			Result.MouseWheel.Position.X = GET_X_LPARAM(LParam);
			Result.MouseWheel.Position.Y = GET_Y_LPARAM(LParam);
			break;
		}

		default:
			// Not an input message
			break;
	}

	return Result;
}
