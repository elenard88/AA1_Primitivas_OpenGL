#version 440 core

uniform vec2 windowSize;

out vec4 fragColor;

void main() {

        if(gl_FragCoord.y > windowSize.y * 0.5){
            fragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f); //Amarillo
        } 
        else {
            fragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f); //Naranja
        }

}
