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
		MessageDigest md = MessageDigest.getInstance("SHA1");
        byte[] buffer = new byte[4096];
        int num;

        try
		{
			while ((num = input.read(buffer)) > 0)
			{
				md.update(buffer, 0, num);
			}
			
			return md;
        }
		finally
		{
            input.close();
        }
    }

    public static void main(String[] args)
	{
		FileOutputStream outputFile = null;

        if(args.length != 4)
		{
			System.err.println("Usage: SignFile publickey.x509.pem privatekey.pk8 inputfile outputfile");
			System.exit(1);
        }

		try 
		{
			X509Certificate publicKey = readPublicKey(new File(args[0]));
			PrivateKey privateKey = readPrivateKey(new File(args[1]));
			outputFile = new FileOutputStream(args[3]);
			
			MessageDigest sha = sha1Sum(new File(args[2]));
			
            Signature signature = Signature.getInstance("SHA1withRSA");
            signature.initSign(privateKey);


			byte[] msgDigest = sha.digest();

			for(int i=0; i<msgDigest.length; i++)
			{
				String str = Integer.toHexString((int)msgDigest[i]&0xff);
        		System.out.print(str);
    		}
			System.out.print("\r\n");
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(1);
        }
		finally
		{
			try
			{
                if (outputFile != null)
					outputFile.close();
            }
			catch (IOException e)
			{
                e.printStackTrace();
                System.exit(1);
            }
        }

		System.err.println("test");
		System.exit(0);
	}
}


static private void dump(String title, byte[] bytes)
    {
        System.out.printf("%s : ", title);
        for (byte b : bytes)
        {
            System.out.printf("%02x ", b & 0xff);
        }
        System.out.println();
    }
 
    public static void main(String[] args) throws Exception
    {
 
        final int keyLength = 512;
        final KeyPairGenerator rsaKeyPairGenerator = KeyPairGenerator.getInstance("RSA");
        rsaKeyPairGenerator.initialize(keyLength);
        KeyPair keyPair = rsaKeyPairGenerator.generateKeyPair();
 
        byte[] document =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1
        };
        dump("Document .............", document);
 
        MessageDigest digestor = MessageDigest.getInstance("SHA1");
        byte[] documentDigest = digestor.digest(document);
        dump("Document digest ......", documentDigest);
 
        // Generate the signature of the document using the Signature class
        Signature s = Signature.getInstance("SHA1withRSA");
        s.initSign(keyPair.getPrivate());
        s.update(document);
        byte[] signature = s.sign();
        dump("Signature ............", signature);
 
        // Decrypt the signature
      final Cipher cipher = Cipher.getInstance("RSA/ECB/NoPadding");
         cipher.init(Cipher.DECRYPT_MODE, keyPair.getPublic());
        byte[] decryptedSignature = cipher.doFinal(signature);
        dump("Decrypted signature ..", decryptedSignature);
 
    }
