# $Id$
#
from inc_cfg import *

test_param = TestParam(
		"Callee=mandatory SRTP, caller=mandatory SRTP",
		[
			InstanceParam("callee", "--null-audio --use-srtp=2 --srtp-secure=0 --max-calls=1"),
			InstanceParam("caller", "--null-audio --use-srtp=2 --srtp-secure=0 --max-calls=1")
		]
		)
