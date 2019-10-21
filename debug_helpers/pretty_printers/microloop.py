import gdb.printing


class BufferPrinter:
    def __init__(self, val):
        self.val = val

    def display_hint(self):
        return 'string'

    def to_string(self):
        str = self.val['buf'].cast(gdb.Type.pointer(gdb.lookup_type('char'))).string()
        return str

    def children(self):
        yield 'size', int(self.val['sz'])


def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("micro_debug_helpers/microloop")
    pp.add_printer('Buffer', '^microloop::Buffer$', BufferPrinter)

    return pp


gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    build_pretty_printer())
