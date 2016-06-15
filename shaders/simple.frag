#version 150

in vec2 fragTexCoord;
uniform sampler2D material;
out vec4 out_Color;

void main() {
	out_Color = texture(material, fragTexCoord);
}
