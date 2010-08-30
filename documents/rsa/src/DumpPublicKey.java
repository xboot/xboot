import java.io.FileInputStream;
import java.math.BigInteger;
import java.security.cert.CertificateFactory;
import java.security.cert.Certificate;
import java.security.KeyStore;
import java.security.Key;
import java.security.PublicKey;
import java.security.interfaces.RSAPublicKey;
import sun.misc.BASE64Encoder;

/**
 * Command line tool to extract RSA public keys from X.509 certificates
 * and output source code with data initializers for the keys.
 */
class DumpPublicKey {
    /**
     * @param key to perform sanity checks on
     * @throws Exception if the key has the wrong size or public exponent
     */
    static void check(RSAPublicKey key) throws Exception
	{
        BigInteger pubexp = key.getPublicExponent();
        BigInteger modulus = key.getModulus();

        if (!pubexp.equals(BigInteger.valueOf(3)))
                throw new Exception("Public exponent should be 3 but is " + pubexp.toString(10) + ".");

        if (modulus.bitLength() != 2048)
             throw new Exception("Modulus should be 2048 bits long but is " + modulus.bitLength() + " bits.");
    }

    /**
     * @param key to output
     * @return a C initializer representing this public key.
     */
    static String print(RSAPublicKey key) throws Exception
	{
        check(key);

        BigInteger N = key.getModulus();

        StringBuilder result = new StringBuilder();

        result.append("struct rsa_public_key public_key = {\r");

        int nwords = N.bitLength() / 32;					/* 32 bit integers in modulus */
		
		result.append("\t");
        result.append(nwords);
		result.append(",\r");

        BigInteger B = BigInteger.valueOf(0x100000000L);	/* 2^32 */
        BigInteger N0inv = B.subtract(N.modInverse(B));		/* -1 / N[0] mod 2^32 */

        result.append("\t0x");
        result.append(N0inv.toString(16));
		result.append(",\r");

        BigInteger R = BigInteger.valueOf(2).pow(N.bitLength());
        BigInteger RR = R.multiply(R).mod(N);				/* 2^4096 mod N */

        /* Write out modulus as little endian array of integers. */
        result.append("\t{\r");

        for (int i = 0; i < nwords; ++i)
		{
            long n = N.mod(B).longValue();
			
			if( i % 4 == 0)
			{
				result.append("\t\t");
			}
			else
			{
				result.append(" ");
			}

            result.append(n);
			result.append("UL");

            if (i != nwords - 1)
			{
                result.append(",");
            }

			if( i % 4 == 3)
			{
				result.append("\r");
			}

            N = N.divide(B);
        }
        result.append("\t},\r");

        /* Write R^2 as little endian array of integers. */
        result.append("\t{\r");

        for (int i = 0; i < nwords; ++i)
		{
            long rr = RR.mod(B).longValue();

			if( i % 4 == 0)
			{
				result.append("\t\t");
			}
			else
			{
				result.append(" ");
			}

            result.append(rr);
			result.append("UL");

            if (i != nwords - 1)
			{
                result.append(",");
            }

			if( i % 4 == 3)
			{
				result.append("\r");
			}

            RR = RR.divide(B);
        }
        result.append("\t}\r");

        result.append("};\r");
        return result.toString();
    }

    public static void main(String[] args)
	{
        if (args.length != 1)
		{
            System.err.println("Usage: DumpPublicKey certfile > source.c");
            System.exit(1);
        }

        try
		{
			FileInputStream input = new FileInputStream(args[0]);
			CertificateFactory cf = CertificateFactory.getInstance("X.509");
			Certificate cert = cf.generateCertificate(input);
			RSAPublicKey key = (RSAPublicKey) (cert.getPublicKey());
			check(key);
			System.out.print(print(key));
        }
		catch (Exception e)
		{
            e.printStackTrace();
            System.exit(1);
        }

        System.exit(0);
    }
}
