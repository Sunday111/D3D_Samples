from build_tools import *

mem_prof_opts = []
mem_prof_opts.append("-DKENG_KengGraphics=OFF")
mem_prof_opts.append("-DKENG_KengResourceSystem=OFF")
mem_prof_opts.append("-DKENG_KengWindowSystem=OFF")
mem_prof_opts.append("-DPROJECT_SimpleQuad=OFF")
mem_prof_opts.append("-DPROJECT_TexturedQuad=OFF")
mem_prof_opts.append("-DPROJECT_RenderText=OFF")
mem_prof_opts.append("-DPROJECT_MemoryProfiling=ON")

delete_build()
use_mm_opts = mem_prof_opts
use_mm_opts.append("-DKENG_KengMemory=ON")
generate_project(use_mm_opts)
build_project()
durationWithMM = run_launcher(args = ['MemoryProfiling'])

delete_build()
use_mm_opts = mem_prof_opts
use_mm_opts.append("-DKENG_KengMemory=OFF")
generate_project(use_mm_opts)
build_project()
durationWithoutMM = run_launcher(args = ['MemoryProfiling'])

print("Duration with memory manager: {0}".format(durationWithMM))
print("Duration without memory manager: {0}".format(durationWithoutMM))