#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Debug: why vectors?
#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// First Person Shooter Like(FPS), Focus on object and translate around(SEEK)
enum Camera_Mode{
    FPS,
    SEEK
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    Camera_Mode acMode;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        acMode = FPS;
        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        acMode = FPS;
        updateCameraVectors();
    }

    void reset(){
        Position = glm::vec3(0.0f, 0.0f, 3.0f);
        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        Yaw = YAW;
        Pitch = PITCH;
        Front = glm::vec3(0.0f, 0.0f, -1.0f);
        Zoom = ZOOM;
        acMode = FPS;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 lookAtClose2GL(){
        glm::vec3 zaxis = glm::normalize(-Front);
        glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(Up), zaxis));
        glm::vec3 yaxis = glm::cross(zaxis, xaxis);

        // Create translation and rotation matrix
        // Warning mat[col][row] due to column-major layout
        glm::mat4 translation = glm::mat4(1.0f);
        translation[3][0] = -Position.x;
        translation[3][1] = -Position.y;
        translation[3][2] = -Position.z;
        glm::mat4 rotation = glm::mat4(1.0f);
        // First column, first row
        rotation[0][0] = xaxis.x;
        rotation[1][0] = xaxis.y;
        rotation[2][0] = xaxis.z;
        // First column, second row
        rotation[0][1] = yaxis.x;
        rotation[1][1] = yaxis.y;
        rotation[2][1] = yaxis.z;
        // First column, third row
        rotation[0][2] = zaxis.x;
        rotation[1][2] = zaxis.y;
        rotation[2][2] = zaxis.z;

        return rotation * translation;
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;

        if(acMode == SEEK){
            updateCameraVectors();
        }
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        if (acMode == SEEK){
            return;
        }
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 0.01f && Zoom <= 89.0f)
            Zoom -= yoffset;
        if (Zoom <= 0.01f)
            Zoom = 0.01f;
        if (Zoom >= 89.0f)
            Zoom = 89.0f;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        if (acMode == SEEK){
            Front = glm::normalize((0.0f, 0.0f, 0.0f) - Position);

        }
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif

