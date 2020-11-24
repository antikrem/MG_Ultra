/**
 * Some helpful conversions between different colour spaces
 */

 /**
  * Converts from hsl in range (0-1, 0-1, 0-1)
  */
 vec3 hsl2rgb(vec3 color) {
    vec3 rgb = clamp( 
			abs(mod(color.x * 6.0+vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 
			0.0, 
			1.0
		);
    return color.z + color.y * (rgb - 0.5) * (1.0 - abs(2.0 * color.z - 1.0));
}
