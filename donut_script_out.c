output_index;double sin()
,cos();main(){float rotation_x=
0,rotation_y=0,torus_angle,circle_angle,depth_buffer[1760];char char_buffer[
1760];printf("\x1b[2J");for(;;
){memset(char_buffer,32,1760);memset(depth_buffer,0,7040)
;for(circle_angle=0;6.28>circle_angle;circle_angle+=0.07)for(torus_angle=0;6.28
>torus_angle;torus_angle+=0.02){float sin_torus=sin(torus_angle),cos_circle=cos(circle_angle),sin_rot_x=
sin(rotation_x),sin_circle=sin(circle_angle),cos_rot_x=cos(rotation_x),radius_offset=cos_circle+2,depth_factor=1/(sin_torus*
radius_offset*sin_rot_x+sin_circle*cos_rot_x+5),cos_torus=cos      (torus_angle),cos_rot_y=cos(rotation_y),sin_rot_y=s\
in(rotation_y),temp_calc=sin_torus*radius_offset*cos_rot_x-sin_circle*        sin_rot_x;int screen_x=40+30*depth_factor*
(cos_torus*radius_offset*cos_rot_y-temp_calc*sin_rot_y),screen_y=            12+15*depth_factor*(cos_torus*radius_offset*sin_rot_y
+temp_calc*cos_rot_y),buffer_index=screen_x+80*screen_y,          lighting=8*((sin_circle*sin_rot_x-sin_torus*cos_circle*cos_rot_x
)*cos_rot_y-sin_torus*cos_circle*sin_rot_x-sin_circle*cos_rot_x-cos_torus        *cos_circle*sin_rot_y);if(22>screen_y&&
screen_y>0&&screen_x>0&&80>screen_x&&depth_factor>depth_buffer[buffer_index]){depth_buffer[buffer_index]=depth_factor;;;char_buffer[buffer_index]=
".,-~:;=!*#$@"[lighting>0?lighting:0];}}/*#****!!-*/
printf("\x1b[H");for(output_index=0;1761>output_index;output_index++)
putchar(output_index%80?char_buffer[output_index]:10);rotation_x+=0.04;rotation_y+=
0.02;}}/*****####*******!!=;:~
~::==!!!**********!!!==::-
.,~~;;;========;;;:~-.
    ..,--------,*/
