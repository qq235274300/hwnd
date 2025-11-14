#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
    return keystates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey()  noexcept
{
    if (keybuffer.size() > 0)
    {
        Keyboard::Event e = keybuffer.front();
        keybuffer.pop();
        return e; 
    }
    return {};
}

bool Keyboard::KeyIsEmpty() const noexcept
{
    return keybuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
    keybuffer = std::queue<Event>();
}

std::optional<char> Keyboard::ReadChar()   noexcept
{
    if (charbuffer.size() > 0)
    {
        char c = charbuffer.front();
        charbuffer.pop();
        return c;
    }

    return {};
}

bool Keyboard::CharIsEmpty() const noexcept
{
    return charbuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
    charbuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
    FlushKey();
    FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept
{
    autorepeatEnable = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
    autorepeatEnable = false;
}

bool Keyboard::AutoRepeatIsEnable() const noexcept
{
    return autorepeatEnable;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
    keystates[keycode] = true;
    keybuffer.push(Keyboard::Event(Event::Type::Press, keycode));
    TrimBuffer(keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Event::Type::Release, keycode));
	TrimBuffer(keybuffer);
}

void Keyboard::OnChar(char character) noexcept
{
    charbuffer.push(character);
	TrimBuffer(charbuffer);
}

void Keyboard::ClearState() noexcept
{
    keystates.reset();
}

template<typename T>
 void Keyboard::TrimBuffer(std::queue<T> buffer) noexcept
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}
