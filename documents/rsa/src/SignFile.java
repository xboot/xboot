import sun.misc.BASE64Encoder;
import sun.security.pkcs.ContentInfo;
import sun.security.pkcs.PKCS7;
import sun.security.pkcs.SignerInfo;
import sun.security.x509.AlgorithmId;
import sun.security.x509.X500Name;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.security.AlgorithmParameters;
import java.security.DigestOutputStream;
import java.security.GeneralSecurityException;
import java.security.Key;
import java.security.KeyFactory;
import java.security.MessageDigest;
import java.security.PrivateKey;
import java.security.Signature;
import java.security.SignatureException;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.KeySpec;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.Enumeration;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.jar.Attributes;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.jar.JarOutputStream;
import java.util.jar.Manifest;
import java.util.regex.Pattern;
import javax.crypto.Cipher;
import javax.crypto.EncryptedPrivateKeyInfo;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;

/**
 * Command line tool to sign file, using SHA1 and RSA keys.
 */
class SignFile
{
    private static X509Certificate readPublicKey(File file)
            throws IOException, GeneralSecurityException
	{
        FileInputStream input = new FileInputStream(file);

        try
		{
            CertificateFactory cf = CertificateFactory.getInstance("X.509");
            return (X509Certificate) cf.generateCertificate(input);
        }
		finally
		{
            input.close();
        }
    }

    /**
     * Reads the password from stdin and returns it as a string.
     *
     * @param keyFile The file containing the private key.  Used to prompt the user.
     */
    private static String readPassword(File keyFile)
	{
        // TODO: use Console.readPassword() when it's available.
        System.out.print("Enter password for " + keyFile + " (password will not be hidden): ");
        System.out.flush();
        BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
        try
		{
            return stdin.readLine();
        } catch (IOException ex)
		{
            return null;
        }
    }

    /**
     * Decrypt an encrypted PKCS 8 format private key.
     *
     * Based on ghstark's post on Aug 6, 2006 at
     * http://forums.sun.com/thread.jspa?threadID=758133&messageID=4330949
     *
     * @param encryptedPrivateKey The raw data of the private key
     * @param keyFile The file containing the private key
     */
    private static KeySpec decryptPrivateKey(byte[] encryptedPrivateKey, File keyFile)
            throws GeneralSecurityException
	{
        EncryptedPrivateKeyInfo epkInfo;

        try
		{
            epkInfo = new EncryptedPrivateKeyInfo(encryptedPrivateKey);
        }
		catch (IOException ex)
		{
            // Probably not an encrypted key.
            return null;
        }

        char[] password = readPassword(keyFile).toCharArray();

        SecretKeyFactory skFactory = SecretKeyFactory.getInstance(epkInfo.getAlgName());
        Key key = skFactory.generateSecret(new PBEKeySpec(password));

        Cipher cipher = Cipher.getInstance(epkInfo.getAlgName());
        cipher.init(Cipher.DECRYPT_MODE, key, epkInfo.getAlgParameters());

        try
		{
            return epkInfo.getKeySpec(cipher);
        }
		catch (InvalidKeySpecException ex)
		{
            System.err.println("SignFile: Password for " + keyFile + " may be bad.");
            throw ex;
        }
    }

    /** Read a PKCS 8 format private key. */
    private static PrivateKey readPrivateKey(File file)
            throws IOException, GeneralSecurityException
	{
        DataInputStream input = new DataInputStream(new FileInputStream(file));
    
	    try
		{
            byte[] bytes = new byte[(int) file.length()];
            input.read(bytes);

            KeySpec spec = decryptPrivateKey(bytes, file);
            if (spec == null)
			{
                spec = new PKCS8EncodedKeySpec(bytes);
            }

            try
			{
                return KeyFactory.getInstance("RSA").generatePrivate(spec);
            }
			catch (InvalidKeySpecException ex)
			{
                return KeyFactory.getInstance("DSA").generatePrivate(spec);
            }
		}
		finally
		{
            input.close();
        }
    }

    private static MessageDigest sha1Sum(File file)
            throws IOException, GeneralSecurityException
	{
		DataInputStream input = new DataInputStream(new FileInputStream(file));
		MessageDigest sha = MessageDigest.getInstance("SHA1");
        byte[] buffer = new byte[4096];
        int num;

        try
		{
			while ((num = input.read(buffer)) > 0)
			{
				sha.update(buffer, 0, num);
			}
			
			return sha;
        }
		finally
		{
            input.close();
        }
    }

    private static Signature sha1WithRsa(File file, PrivateKey privateKey)
            throws IOException, GeneralSecurityException
	{
		DataInputStream input = new DataInputStream(new FileInputStream(file));
        byte[] buffer = new byte[4096];
        int num;

		Signature sig = Signature.getInstance("SHA1withRSA");
		sig.initSign(privateKey);

        try
		{
			while ((num = input.read(buffer)) > 0)
			{
				sig.update(buffer, 0, num);
			}
			
			return sig;
        }
		finally
		{
            input.close();
        }
    }

    private static void writeFile(File file, byte[] array)
            throws IOException, GeneralSecurityException
	{
        FileOutputStream output = new FileOutputStream(file);

        try
		{
			output.write(array);
        }
		finally
		{
            output.close();
        }
    }

	static private void dump(byte[] bytes)
    {
        for (byte b : bytes)
        {
            System.out.printf("0x%02x,", b & 0xff);
        }
        System.out.println();
    }

    public static void main(String[] args)
	{
        if(args.length != 4)
		{
			System.err.println("Usage: SignFile public.x509.pem private.pk8 input output");
			System.exit(1);
        }

		try 
		{
			X509Certificate publicKey = readPublicKey(new File(args[0]));
			PrivateKey privateKey = readPrivateKey(new File(args[1]));
			
            Signature s = sha1WithRsa(new File(args[2]), privateKey);
			writeFile(new File(args[3]), s.sign());
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(1);
        }

		System.exit(0);
	}
}

