#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        //TODO - Update the rest of camera parameters

        // Inițializează direcția inițială a camerei  - directia frontala - axa z 
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition); 

        // Inițializează direcția către dreapta
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)); 

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
        // sau asa
        // return glm::lookAt(cameraPosition, (cameraPosition + cameraFrontDirection), cameraUpDirection); 
    }

    void Camera::setCameraPosition(glm::vec3 cameraPos) {
        cameraPosition = cameraPos; 
    } 
     
    void Camera::setCameraFrontDirection(glm::vec3 cameraFront) {
        cameraFrontDirection = cameraFront; 
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction)
        {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            cameraTarget += cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            cameraTarget -= cameraFrontDirection * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            cameraTarget += cameraRightDirection * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            cameraTarget -= cameraRightDirection * speed;
            break;
        } 
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO

        cameraTarget.x = cameraPosition.x + cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraTarget.y = cameraPosition.y + sin(glm::radians(pitch));
        cameraTarget.z = cameraPosition.z + sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }
}