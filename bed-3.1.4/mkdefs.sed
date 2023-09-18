s/AC_DEFINE(\([^),]*\))/\
#undef \1\
/g
s/AC_DEFINE(\([^),]*\),\([^)]*\))/\
#undef \1\
/g
