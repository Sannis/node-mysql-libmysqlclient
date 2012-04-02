{
  'targets': [
    {
      # have to specify 'liblib' here since gyp will remove the first one :\
      'target_name': 'mysql_bindings',
      'sources': [
        'src/mysql_bindings.cc',
        'src/mysql_bindings_connection.cc',
        'src/mysql_bindings_result.cc',
        'src/mysql_bindings_statement.cc',
      ],
      'conditions': [
        ['OS=="win"', {
          # no Windows support yet...
        }, {
          'libraries': [
            '<!@(mysql_config --libs_r)'
          ],
        }],
        ['OS=="mac"', {
          # cflags on OS X are stupid and have to be defined like this
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '<!@(mysql_config --cflags)'
            ]
          }
        }, {
          'cflags': [
            '<!@(mysql_config --cflags)'
          ],
        }]
      ]
    }
  ]
}