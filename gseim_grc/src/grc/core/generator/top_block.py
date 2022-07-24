import codecs
import operator
import os
import tempfile
import textwrap

from .. import Messages, blocks
from .FlowGraphProxy import FlowGraphProxy
from ..utils import expr_utils

DATA_DIR = os.path.dirname(__file__)

class TopBlockGenerator(object):

    def __init__(self, flow_graph, file_path):
        """
        Initialize the top block generator object.

        Args:
            flow_graph: the flow graph object
            file_path: the path to write the file to
        """

        self._flow_graph = FlowGraphProxy(flow_graph)
        self._generate_options = self._flow_graph.get_option('generate_options')

        # Handle the case where the directory is read-only
        # In this case, use the system's temp directory
        if not os.access(file_path, os.W_OK):
            file_path = tempfile.gettempdir()
        filename = self._flow_graph.get_option('id') + '.py'
        self.file_path = os.path.join(file_path, filename)
        self._dirname = file_path
        print("TopBlockGenerator: filename =",filename)
        print("TopBlockGenerator: file_path =",file_path)

    def write(self):
        """generate output and write it to files"""

        fg = self._flow_graph
        self.title = fg.get_option('title') or fg.get_option('id').replace('_', ' ').title()
        variables = fg.get_variables()
        parameters = fg.get_parameters()

        self.namespace = {
            'flow_graph': fg,
            'variables': variables,
            'parameters': parameters,
            'generate_options': self._generate_options,
        }

