varying mediump vec2 vCoord;
uniform mediump mat2 textureTransform;
uniform mediump vec2 half_offset;

void main(void) {
	mediump vec2 texCoord = mod(textureTransform * (vCoord.xy + 0.5 * half_offset), 1.0);
	texCoord = mix(texCoord, 1.0 - texCoord, step(0.5, texCoord.x));
	mediump float stringness = abs(1.0 - 2.0 * mod(2.0 * texCoord.y, 1.0));
	if (stringness < 0.5) { discard; }
	gl_FragColor = vec4(texCoord, 0.5, 1.0);
}
