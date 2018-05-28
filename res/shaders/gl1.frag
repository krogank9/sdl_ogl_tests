precision mediump float;
varying vec4 vertex_color;
varying vec3 ourColor;

void main(void) {
	gl_FragColor = vec4(ourColor, 1.0);
}
