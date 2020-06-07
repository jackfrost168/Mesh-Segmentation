// Timing.h


typedef LARGE_INTEGER timestamp;


float LI2f(const LARGE_INTEGER &li);

void get_timestamp(timestamp &now);

float operator - (const timestamp &t1, const timestamp &t2);
