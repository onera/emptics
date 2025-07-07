import matplotlib # FIXME necessary for `import matplotlib` to work on adastra
import emptics

print("Test sample functions")
emptics.print_hello()
emptics.call_blas()

print("Test all imports")
from emptics import all_imports
