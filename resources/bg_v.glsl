//Passthrough vertex shader for dummy quad
#version 120
void main() {
    gl_Position = ftransform();
}
