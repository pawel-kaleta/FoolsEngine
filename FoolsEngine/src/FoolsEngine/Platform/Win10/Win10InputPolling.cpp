#include "FE_pch.h"
#include "Win10InputPolling.h"

#include "FoolsEngine\Core\Application.h"
#include <GLFW\glfw3.h>

namespace fe {

#ifdef FE_PLATFORM_WINDOWS
    Scope<InputPolling> InputPolling::s_Instance = CreateScope<Win10InputPolling>();
#endif
    
    bool Win10InputPolling::IsKeyPressedNative(int keycode)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Win10InputPolling::IsMouseButtonPressedNative(int button)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> Win10InputPolling::GetMousePositionNative()
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return { x, y };
    }

    float Win10InputPolling::GetMouseXNative()
    {
        auto[x, y] = GetMousePositionNative();
        return x;
    }

    float Win10InputPolling::GetMouseYNative()
    {
        auto[x, y] = GetMousePositionNative();
        return y;
    }

}

