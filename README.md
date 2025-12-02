<div align="center">

# libsconf - Lightweight S-expression parser 

[![GitHub License](https://img.shields.io/github/license/d0p1s4m4/libsconf?style=flat-square&logo=opensourceinitiative&logoColor=white)
](LICENSE)
![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/d0p1s4m4/libsconf/check.yml?branch=master&style=flat-square)


</div>

libsconf is a small, simple C99 library to parse and manipulate
S-expressions.

## Building

```
$ autoreconf -i -f
$ ./configure
$ make
$ make check # run test suite
$ sudo make install
```

## Basic Usage

### Parsing

From a string:
```c
const char *str = "(name \"libsconf\")";

struct sconf *root = sconf_parse(str);
sconf_dump(stdout, root);
/* → (name "libsconf") */

sconf_destroy(root);
```

From a file:
```c
FILE *fp = fopen("config.conf", "r");
struct sconf *root = sconf_load(fp);
fclose(fp);

if (root == NULL)
{
	fprintf(stderr, "error: %s\n",
		sconf_error_str(sconf_get_last_error()));
}
```

### Creating expressions manually

```c
struct sconf *lst = sconf_new_list();
struct sconf *name = sconf_new_symbol("username");
struct sconf *str = sconf_new_string("d0p1");

sconf_list_appends(lst, name, str, SCONF_END);

sconf_dump(stdout, lst);
/* → (username "d0p1") */

sconf_destroy(lst);
```

## License

<img src="https://opensource.org/wp-content/themes/osi/assets/img/osi-badge-light.svg" align="right" height="128px" alt="OSI Approved License">

libsconf is licensed under the 3-Clause BSD License.

The full text of the license can be accessed via [this link](https://opensource.org/licenses/BSD-3-Clause) and is also included in [the license file](LICENSE) of this software package.