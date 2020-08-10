varying mediump vec2 vCoord;
uniform mediump mat2 textureTransform;
uniform mediump vec2 half_offset;

void main(void) {
	mediump vec2 texCoord = mod(textureTransform * (vCoord.xy + 0.5 * half_offset), 1.0);
	texCoord = mix(texCoord, 1.0 - texCoord, step(0.5, texCoord.x));
	mediump float stringness = abs(1.0 - 2.0 * mod(2.0 * texCoord.y, 1.0));
	mediump float string_thickness = 16.0 * texCoord.x * (0.5 - texCoord.x);
	string_thickness = max(0.5 * sqrt(string_thickness), 1e-3);
	if (stringness < 1.0 - string_thickness) { discard; }

	mediump vec2 color_coord = 2.0 * texCoord;
	color_coord.y = min(color_coord.y, 2.0 - color_coord.y);
	lowp vec4 color = mix(
		vec4(0.0, 0.8 - 0.4 * color_coord.x, 0.4 + 0.4 * color_coord.x, 1.0),
		vec4(1.0, 0.4 + 0.4 * color_coord.x, 0.0, 1.0),
		step(0.5, color_coord.y)
	);

	gl_FragColor = color;
}
