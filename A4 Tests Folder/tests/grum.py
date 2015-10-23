#!/usr/bin/env python
from __future__ import print_function
import sys
import os

# Get correct path to files, based on platform
import platform
host = platform.node().split('.')[0]

if host=="moss":
   sys.path[0:0]=['/local/courses/csse2310/lib']
   TEST_LOCATION = '/home/users/uqjfenw1/public/a4ptest'
else:
	sys.path[0:0] = [ 
	    '/home/joel/marks',
	]
	TEST_LOCATION = '/home/joel/csse2310_2015/ass4/ptests'
import marks

COMPILE = "make"
 
 
class Assignment4(marks.TestCase):
  timeout = 12 
  @classmethod
  def setup_class(cls):
        # Store original location
        options = getattr(cls, '__marks_options__', {})
	cls.prog = os.path.join(options['working_dir'], './station')
        cls._compile_warnings = 0
        cls._compile_errors = 0
 
        # Create symlink to tests in working dir
        os.chdir(options['working_dir'])
        try:
            os.symlink(TEST_LOCATION, 'tests')
        except OSError:
            pass
        os.chdir(options['temp_dir'])

        # Modify test environment when running tests (excl. explain mode).
        if not options.get('explain', False):
            # Compile program
            os.chdir(options['working_dir'])
            p = marks.Process(COMPILE.split())
            os.chdir(options['temp_dir'])
 
            # Count warnings and errors
            warnings = 0
            errors = 0
            while True:
                line = p.readline_stderr()
                if line == '':
                    break
                if 'warning:' in line:
                    warnings += 1
                if 'error:' in line:
                    errors += 1
                print(line, end='')

            # Do not run tests if compilation failed.
            assert p.assert_exit_status(0)
 
            # Create symlink to tests within temp folder
            try:
                os.symlink(TEST_LOCATION, 'tests')
            except OSError:
                pass 

#bad arguments
  @marks.marks('Argument checking', category_marks=5)
  def test_args1(self):
    """ """
    p = self.process([self.prog], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/usage.err')
    self.assert_exit_status(p, 1)

  @marks.marks('Argument checking', category_marks=5)
  def test_args2(self):
    """ """
    p = self.process([self.prog, 'me'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/usage.err')
    self.assert_exit_status(p, 1)

  @marks.marks('Argument checking', category_marks=5)
  def test_args3(self):
    """ """
    p = self.process([self.prog, 'me', 'notthere'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/usage.err')
    self.assert_exit_status(p, 1)

  @marks.marks('Argument checking', category_marks=5)
  def test_args4(self):
    """ """
    p = self.process([self.prog, '', 'tests/auth1', 'mylog'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/auth.err')
    self.assert_exit_status(p, 2)

  @marks.marks('Argument checking', category_marks=5)
  def test_args5(self):
    """ """
    p = self.process([self.prog, 'me', 'notthere', 'mylog'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/auth.err')
    self.assert_exit_status(p, 2)

  @marks.marks('Argument checking', category_marks=5)
  def test_args6(self):
    """ """
    p = self.process([self.prog, 'me', 'tests/emptyline', 'mylog'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/auth.err')
    self.assert_exit_status(p, 2)

  @marks.marks('Argument checking', category_marks=5)
  def test_args7(self):
    """ """
    p = self.process([self.prog, 'me', 'tests/empty', 'mylog'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/auth.err')
    self.assert_exit_status(p, 2)

  @marks.marks('Argument checking', category_marks=5)
  def test_args8(self):
    """Lets look at ports """
    p = self.process([self.prog, 'me', 'tests/auth1', 'mylog', '0'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/port.err')
    self.assert_exit_status(p, 4)

  @marks.marks('Argument checking', category_marks=5)
  def test_args9(self):
    """Lets look at ports """
    p = self.process([self.prog, 'me', 'tests/auth1', 'mylog', '65535'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/port.err')
    self.assert_exit_status(p, 4)

  @marks.marks('Argument checking', category_marks=5)
  def test_args10(self):
    """Lets look at ports """
    p = self.process([self.prog, 'me', 'tests/auth1', 'mylog', '3204batman'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/port.err')
    self.assert_exit_status(p, 4)

  @marks.marks('Argument checking', category_marks=5)
  def test_args11(self):
    """ listening """
    p = self.process([self.prog, 'me', 'tests/auth1', 'mylog', '10'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/listen.err')
    self.assert_exit_status(p, 5)

  @marks.marks('Argument checking', category_marks=5)
  def test_args12(self):
    """ listening """
    p = self.process([self.prog, 'me', 'tests/auth1', 'mylog', '10', 'localhost4'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/listen.err')
    self.assert_exit_status(p, 5)

  @marks.marks('Argument checking', category_marks=5)
  def test_args13(self):
    """ listening """
    p = self.process([self.prog, 'me', 'tests/auth1', 'mylog', '10', '127.0.0.1'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/listen.err')
    self.assert_exit_status(p, 5)

  @marks.marks('Argument checking', category_marks=5)
  def test_args14(self):
    """ listening """
    p = self.process([self.prog, 'me', 'tests/auth1', 'mylog', '3500', '126.0.0.1'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/listen.err')
    self.assert_exit_status(p, 5)

  @marks.marks('Argument checking', category_marks=5)
  def test_args14(self):
    """ logfile """
    p = self.process([self.prog, 'me', 'tests/auth1', '/zzz', '3500', '126.0.0.1'], 'tests/empty' )
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/log.err')
    self.assert_exit_status(p, 3)


if __name__ == '__main__':
    marks.main()

