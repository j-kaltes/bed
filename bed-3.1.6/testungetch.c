namespace conftest {
  extern "C" int PDC_ungetch ();
}
int
main (void)
{
return conftest::PDC_ungetch ();
  ;
  return 0;
}
