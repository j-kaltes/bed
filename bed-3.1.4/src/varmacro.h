/* In exactly what version did it change? */
#if defined(__GNUC__) && (__GNUC__ < 2  || (__GNUC__==2 &&__GNUC_MINOR__ <= 95))
/* What else could I have intended  */
#define VARMACRO
#define OLDGCCVARMACRO
#endif
