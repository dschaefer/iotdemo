/*******************************************************************************
 * Copyright (c) 2016 QNX Software Systems and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package doug.iotdemo.mojo.deployer;

import java.io.File;

import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugin.MojoFailureException;
import org.apache.maven.plugins.annotations.Mojo;
import org.apache.maven.plugins.annotations.Parameter;

import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.transfer.Transfer;
import com.amazonaws.services.s3.transfer.TransferManager;

@Mojo(name = "deploy")
public class Deployer extends AbstractMojo {

	@Parameter(property = "deployer.source", required = true)
	private File source;

	@Parameter(property = "deployer.bucketName", required = true)
	private String bucketName;

	@Parameter(property = "deployer.bucketKey", required = true)
	private String bucketKey;

	@Override
	public void execute() throws MojoExecutionException, MojoFailureException {
		AmazonS3 s3 = new AmazonS3Client();

		if (!s3.doesBucketExist(bucketName)) {
			throw new MojoExecutionException("bucket " + bucketName + " does not exist");
		}

		getLog().info("Uploading " + source.getName() + " to s3://" + bucketName + "/" + bucketKey);

		TransferManager manager = new TransferManager(s3);
		Transfer transfer;
		if (source.isFile()) {
			transfer = manager.upload(bucketName, bucketKey, source);
		} else if (source.isDirectory()) {
			transfer = manager.uploadDirectory(bucketName, bucketKey, source, true);
		} else {
			throw new MojoExecutionException("Unknown file type " + source.getAbsolutePath());
		}

		try {
			transfer.waitForCompletion();
		} catch (InterruptedException e) {
			throw new MojoExecutionException("Upload to S3 failed", e);
		}
	}

}
