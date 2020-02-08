# Lab1
By  Xiyuan He (xh31) and  Sue Kim (sbk4)
## File Organization
All the files are divided into four folders, each file in the folder is a txt file and a corresponding image is also provided. By load each txt file, you will get the exact image
### Simple turtle programs
- Polygon
- Star
- InscribedStar
- Wheel
- Rosette
- Spiral
### Turtle Program Operators
- Shift
- Spin
- Scale
### Turtle Fractals
- KochSnowflakeSide
- SnowflakeSide
- Snowflake
- PolyGasket
### Additional Fractals
- Bush
- Hangman
- Square
- Stairs
- Tree
### FLAG
- Flag

  It may take a while to generate the flag (several seconds) !
## Something special
I think there is something special about our implementation on the resize function, so I listed our implementation below:
```C++
void resize(double s){
		_scale *= s;
	}
// we used "*=", instead of "="

```
## As for the star
For the star with even number of vertices, we decided not to modify it.