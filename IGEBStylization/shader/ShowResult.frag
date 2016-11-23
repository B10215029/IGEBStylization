#version 430

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;
uniform sampler2D tex6;
uniform sampler2D tex7;
uniform sampler2D tex8;
uniform sampler2D tex9;

in vec2 texCoord;
out vec4 fragColor;

void main() {
    //vec2 flipTexCoord = vec2(1,-1) * texCoord;
	float x = texCoord.x * 4;
	float y = texCoord.y * 4;
	if (x > 0 && x < 1 && y >3 && y < 4)
		fragColor = texture(tex0, texCoord * 4);
	if (x > 1 && x < 2 && y >3 && y < 4)
		fragColor = texture(tex1, texCoord * 4);
	if (x > 2 && x < 3 && y >3 && y < 4)
		fragColor = texture(tex2, texCoord * 4);
	if (x > 3 && x < 4 && y >3 && y < 4)
		fragColor = texture(tex3, texCoord * 4);

	if (x > 0 && x < 1 && y >0 && y < 1)
		fragColor = texture(tex4, texCoord * 4);
	if (x > 1 && x < 2 && y >0 && y < 1)
		fragColor = texture(tex5, texCoord * 4);
	if (x > 2 && x < 3 && y >0 && y < 1)
		fragColor = texture(tex6, texCoord * 4);
	if (x > 3 && x < 4 && y >0 && y < 1)
		fragColor = texture(tex7, texCoord * 4);

	if (x > 0 && x < 2 && y >1 && y < 3)
		fragColor = texture(tex8, texCoord * 2 + vec2 (0, 0.5));
	if (x > 2 && x < 4 && y >1 && y < 3)
		fragColor = texture(tex9, texCoord * 2 + vec2(0, 0.5));
	//fragColor = vec4(1, 1, 1, 1);

}
