#include "FE_pch.h"
#include "FoolsEngine\Platform\InputPolling.h"

#include "FoolsEngine\Core\Application.h"
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#ifdef FE_PLATFORM_WINDOWS

namespace fe
{
    bool InputPolling::IsKeyPressed(int keycode)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool InputPolling::IsMouseButtonPressed(int button)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    glm::vec2 InputPolling::GetMousePosition()
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return { (float)x, (float)y };
    }

    float InputPolling::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float InputPolling::GetMouseY()
    {
        return GetMousePosition().y;
    }
}

#endif


