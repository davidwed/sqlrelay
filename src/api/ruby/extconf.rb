require 'mkmf'
dir_config('rudiments')
have_library('sqlrclient','')
have_library('rudiments','')
create_makefile('sqlrelay')
