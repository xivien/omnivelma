# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from omnivelma_msgs/Encoders.msg. Do not edit."""
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct

import omnivelma_msgs.msg

class Encoders(genpy.Message):
  _md5sum = "a14e6d34d7cb73a32c3f00a409af72f0"
  _type = "omnivelma_msgs/Encoders"
  _has_header = False #flag to mark the presence of a Header object
  _full_text = """omnivelma_msgs/Vels velocity
omnivelma_msgs/Vels angle

================================================================================
MSG: omnivelma_msgs/Vels
float64 fr
float64 fl
float64 rl
float64 rr"""
  __slots__ = ['velocity','angle']
  _slot_types = ['omnivelma_msgs/Vels','omnivelma_msgs/Vels']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       velocity,angle

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(Encoders, self).__init__(*args, **kwds)
      #message fields cannot be None, assign default values for those that are
      if self.velocity is None:
        self.velocity = omnivelma_msgs.msg.Vels()
      if self.angle is None:
        self.angle = omnivelma_msgs.msg.Vels()
    else:
      self.velocity = omnivelma_msgs.msg.Vels()
      self.angle = omnivelma_msgs.msg.Vels()

  def _get_types(self):
    """
    internal API method
    """
    return self._slot_types

  def serialize(self, buff):
    """
    serialize message into buffer
    :param buff: buffer, ``StringIO``
    """
    try:
      _x = self
      buff.write(_get_struct_8d().pack(_x.velocity.fr, _x.velocity.fl, _x.velocity.rl, _x.velocity.rr, _x.angle.fr, _x.angle.fl, _x.angle.rl, _x.angle.rr))
    except struct.error as se: self._check_types(struct.error("%s: '%s' when writing '%s'" % (type(se), str(se), str(locals().get('_x', self)))))
    except TypeError as te: self._check_types(ValueError("%s: '%s' when writing '%s'" % (type(te), str(te), str(locals().get('_x', self)))))

  def deserialize(self, str):
    """
    unpack serialized message in str into this message instance
    :param str: byte array of serialized message, ``str``
    """
    try:
      if self.velocity is None:
        self.velocity = omnivelma_msgs.msg.Vels()
      if self.angle is None:
        self.angle = omnivelma_msgs.msg.Vels()
      end = 0
      _x = self
      start = end
      end += 64
      (_x.velocity.fr, _x.velocity.fl, _x.velocity.rl, _x.velocity.rr, _x.angle.fr, _x.angle.fl, _x.angle.rl, _x.angle.rr,) = _get_struct_8d().unpack(str[start:end])
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e) #most likely buffer underfill


  def serialize_numpy(self, buff, numpy):
    """
    serialize message with numpy array types into buffer
    :param buff: buffer, ``StringIO``
    :param numpy: numpy python module
    """
    try:
      _x = self
      buff.write(_get_struct_8d().pack(_x.velocity.fr, _x.velocity.fl, _x.velocity.rl, _x.velocity.rr, _x.angle.fr, _x.angle.fl, _x.angle.rl, _x.angle.rr))
    except struct.error as se: self._check_types(struct.error("%s: '%s' when writing '%s'" % (type(se), str(se), str(locals().get('_x', self)))))
    except TypeError as te: self._check_types(ValueError("%s: '%s' when writing '%s'" % (type(te), str(te), str(locals().get('_x', self)))))

  def deserialize_numpy(self, str, numpy):
    """
    unpack serialized message in str into this message instance using numpy for array types
    :param str: byte array of serialized message, ``str``
    :param numpy: numpy python module
    """
    try:
      if self.velocity is None:
        self.velocity = omnivelma_msgs.msg.Vels()
      if self.angle is None:
        self.angle = omnivelma_msgs.msg.Vels()
      end = 0
      _x = self
      start = end
      end += 64
      (_x.velocity.fr, _x.velocity.fl, _x.velocity.rl, _x.velocity.rr, _x.angle.fr, _x.angle.fl, _x.angle.rl, _x.angle.rr,) = _get_struct_8d().unpack(str[start:end])
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e) #most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_8d = None
def _get_struct_8d():
    global _struct_8d
    if _struct_8d is None:
        _struct_8d = struct.Struct("<8d")
    return _struct_8d
