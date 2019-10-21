import gdb.printing


class HttpVersionPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        major = int(self.val['major'])
        minor = int(self.val['minor'])

        return f'HTTP/{major}.{minor}'


class HttpRequestPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return 'HttpRequest'

    def children(self):
        relevant_keys = ('http_version', 'http_method', 'uri')

        for field in self.val.type.fields():
            key = field.name
            val = self.val[key]

            print(val.type);

            if key not in relevant_keys:
                continue

            yield key, val.string()


def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("micro_debug_helpers")
    pp.add_printer('HttpVersion', '^microhttp::http::Version$', HttpVersionPrinter)
    # pp.add_printer('HttpRequest', '^microhttp::http::HttpRequest$', HttpRequestPrinter)

    return pp


gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    build_pretty_printer())
