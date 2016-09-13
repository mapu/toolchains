#ifndef UCPS_INST_FLAGS_H
#define UCPS_INST_FLAGS_H

namespace llvm
{
	namespace UCPS
	{
		namespace SCUFlag
		{
			enum SCUFlag
			{
				T = 0,
				S = 1,
				U = 2, // this order is used for encoding/decoding
				D = 3
			};
		}

		namespace XferFlag
		{
			enum XferFlag {
				U = 0,
				H = 1,
				B = 2
			};
		}

		namespace KMFlag
		{
			enum KMFlag {
				L = 0,
				H = 1
			};
		}

	}

}

#endif // UCPS_INST_FLAGS_H
