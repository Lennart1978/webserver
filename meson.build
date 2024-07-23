project('webserver', 'c', version : '1.0')

inc = include_directories('.')

sources = [
    'service.c',
    'webserver.c'
]

executable('webserver', sources,
    include_directories : inc,
    install : true)
