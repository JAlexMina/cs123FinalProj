/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
#include <Settings.h>
#include <iostream>

CamtransCamera::CamtransCamera()
{
    m_near = 1;
    m_far = 30;
    m_thetaH = 60;
    m_aspectRatio = 1;
    m_thetaW = glm::degrees(2*glm::atan(m_aspectRatio*glm::tan(glm::radians(m_thetaH / 2))));
    m_eye = glm::vec4(2,2,2,1);
    m_up = glm::vec4(0,1,0,0);
    m_look = glm::normalize(glm::vec4(-2,-2,-2,0));
    m_w = -glm::normalize(m_look);
    m_v = glm::normalize(m_up - glm::dot(m_up, m_w)*m_w);
    m_u = glm::normalize(glm::vec4(glm::cross(m_v.xyz(), m_w.xyz()),0));
    updateViewMatrix();
    updateProjectionMatrix();

}

void CamtransCamera::setAspectRatio(float a)
{
    m_aspectRatio = a;
    setHeightAngle(m_thetaH);
    updateProjectionMatrix();
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_perspectiveTransformation * m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_rotationMatrix * m_translationMatrix;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    return m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    return m_perspectiveTransformation;
}

glm::vec4 CamtransCamera::getPosition() const {
    return m_eye;
}

glm::vec4 CamtransCamera::getLook() const {
    return m_look;
}

glm::vec4 CamtransCamera::getUp() const {
    return m_up;
}

glm::vec4 CamtransCamera::getU() const {
    return m_u;

}

glm::vec4 CamtransCamera::getV() const {
    return m_v;

}

glm::vec4 CamtransCamera::getW() const {
    return m_w;

}

float CamtransCamera::getAspectRatio() const {
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    return m_thetaH;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    //  m_look = glm::normalize(glm::vec4(-2,-2,-2,0));
    m_eye = eye;
    m_look = glm::normalize(look);
    m_up = glm::normalize(up);

    m_w = -glm::normalize(m_look);
    m_v = glm::normalize(m_up - glm::dot(m_up, m_w)*m_w);
    m_u = glm::normalize(glm::vec4(glm::cross(m_v.xyz(), m_w.xyz()),0));

    updateViewMatrix();
    updateProjectionMatrix();
}

void CamtransCamera::setHeightAngle(float h) {
    m_thetaH = h;
    m_thetaW = glm::degrees(2*glm::atan(m_aspectRatio*glm::tan(glm::radians(m_thetaH / 2))));

    updateProjectionMatrix();
}

void CamtransCamera::translate(const glm::vec4 &v) {
    m_eye += v;
    updateViewMatrix();
}

void CamtransCamera::rotateU(float degrees) {
    float rads = glm::radians(degrees);
    glm::vec4 savedV = m_v;
    m_v = m_w * glm::sin(rads) + m_v * glm::cos(rads);
    m_w = m_w * glm::cos(rads) - savedV * glm::sin(rads);
    m_look = -m_w;
    updateViewMatrix();
}

void CamtransCamera::rotateV(float degrees) {
    float rads = glm::radians(degrees);
    glm::vec4 savedU = m_u;
    m_u = m_u * glm::cos(rads) - m_w * glm::sin(rads);
    m_w = savedU * glm::sin(rads) + m_w * glm::cos(rads);
    updateViewMatrix();
}

void CamtransCamera::rotateW(float degrees) {
    float rads = glm::radians(-degrees);
    glm::vec4 savedU = m_u;
    m_u = m_u * glm::cos(rads) - m_v * glm::sin(rads);
    m_v = savedU * glm::sin(rads) + m_v * glm::cos(rads);
    m_look = -m_w;
    updateViewMatrix();
}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    m_near = nearPlane;
    m_far = farPlane;
    updateProjectionMatrix();
}

void CamtransCamera::updateProjectionMatrix()
{
    updateScaleMatrix();
    updatePerspectiveMatrix();
}

void CamtransCamera::updatePerspectiveMatrix()
{
    float c = -m_near / m_far;

    m_perspectiveTransformation = glm::transpose(glm::mat4x4(1,0,0,0,
                                                             0,1,0,0,
                                                             0,0,-1.0f/(1+c), c/(1+c),
                                                             0,0,-1,0));
}

void CamtransCamera::updateScaleMatrix()
{
    m_scaleMatrix = glm::transpose(glm::mat4x4(1.0f/(m_far*glm::tan(glm::radians(m_thetaW/2.0f))),0,0,0,
                                               0,1.0f/(m_far*glm::tan(glm::radians(m_thetaH/2.0f))),0,0,
                                               0,0,1.0f/m_far, 0,
                                               0,0,0,1));
}

void CamtransCamera::updateViewMatrix()
{
    updateTranslationMatrix();
    updateRotationMatrix();
}

void CamtransCamera::updateRotationMatrix()
{
    m_rotationMatrix = glm::transpose(glm::mat4x4(m_u[0],m_u[1],m_u[2],0,
                                                  m_v[0],m_v[1],m_v[2],0,
                                                  m_w[0],m_w[1],m_w[2],0,
                                                  0,0,0,1));
}

void CamtransCamera::updateTranslationMatrix()
{
    m_translationMatrix = glm::transpose(glm::mat4x4(1,0,0,-m_eye[0],
                                                     0,1,0,-m_eye[1],
                                                     0,0,1,-m_eye[2],
                                                     0,0,0,1));
}

